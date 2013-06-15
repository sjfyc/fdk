#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Board.h"
#include "ActorBank.h"
#include "AStarRecorder.h"

Game::Game()
	: m_mode(&g_GameModeGame)
{
}

Game::~Game()
{
}

bool Game::start()
{
	if (!fdk::ModuleManager::instance().start())
	{
		util::output(fdk::ModuleManager::instance().getErrorMessage().c_str());
		return false;
	}
	
	outputUsage();

	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);

	m_startCoord.reset(3, CELL_COUNT_Y/2);
	m_targetCoord.reset(CELL_COUNT_X-4, CELL_COUNT_Y/2);	
	return true;
}

void Game::stop()
{
	fdk::EventHook::unregist();

	fdk::ModuleManager::instance().stop();
}

void Game::update(float delta)
{
	m_mode->update(*this, delta);
	if (!fdk::ModuleManager::instance().tick(delta))
	{
		util::output(fdk::ModuleManager::instance().getErrorMessage().c_str());
	}
}

void Game::render()
{
	g_Board.draw();
	util::fillCell(m_startCoord, COLOR_CELL_FROM);
	util::fillCell(m_targetCoord, COLOR_CELL_TO);
	g_ActorBank.draw();
	m_mode->render(*this);
}

void Game::onEvent(int eventType, void* params)
{
	if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		if (key == HGEK_ESCAPE)
		{
			IsRunning = false;
		}
		else if (key == HGEK_M)
		{
			toggleMode();
		}
		else if (key == HGEK_F)
		{
			if (m_mode == &g_GameModeMapEdit)
			{
				toggleMode();
			}
			m_astarRecorder = new AStarRecorder;
			m_astar = new AStar(g_Board, 
				g_Board.getNodeID(m_startCoord),
				g_Board.getNodeID(m_targetCoord),
				m_astarRecorder);			
		}
	}
	m_mode->handleEvent(*this, eventType, params);
}

void Game::toggleMode()
{
	if (m_mode == &g_GameModeGame)
	{
		m_mode = &g_GameModeMapEdit;
	}
	else 
	{
		m_mode = &g_GameModeGame;
	}
	util::output("toggle to %s mode", (m_mode == &g_GameModeGame ? "game" : "map edit") );
}

void Game::outputUsage()
{
	util::output("M: toggle between game & map edit mode");
	util::output("F: start or continue find path");
	util::output("S: pause path finding");
}

void GameModeGame::update(Game& game, float delta)
{
	AStar* astar = game.m_astar;
	if (astar)
	{
		AStar::SearchResult result = astar->search(1);
		if (result == AStar::SearchResult_Completed)
		{
		}
	}
}

void GameModeGame::render(Game& game)
{
	if (game.m_astarRecorder)
	{
		game.m_astarRecorder->render();
	}
	AStar* astar = game.m_astar;
	if (astar && astar->isCompleted())
	{
		const std::vector<int>& pathNodes = astar->getPath();

		CellCoord prevCellCoord = game.m_startCoord;		
		for (size_t i = pathNodes.size()-1; i >= 0; --i)
		{
			Location prevCenterLocation = util::cellCoordToLocation(prevCellCoord);
			prevCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			CellCoord currentCellCoord = g_Board.getNodeCoord(pathNodes[i]);
			Location currentCenterLocation = util::cellCoordToLocation(currentCellCoord);
			currentCenterLocation += Location(CELL_SIZE_X/2, CELL_SIZE_Y/2);

			g_HGE->Gfx_RenderLine(prevCenterLocation.x, prevCenterLocation.y, 
				currentCenterLocation.x, currentCenterLocation.y,
				COLOR_YELLOW
				);
		}
		
	}
}

void GameModeGame::handleEvent(Game& game, int eventType, void* params)
{
}

GameModeMapEdit::GameModeMapEdit()
	: m_bMouseTracking(false)
	, m_lastMouseCoord()
	, m_brush(Brush_Block)
{
}

void GameModeMapEdit::enter(Game& game)
{
	delete game.m_astar;
	game.m_astar = 0;
}

void GameModeMapEdit::leave(Game& game)
{
}

void GameModeMapEdit::handleEvent(Game& game, int eventType, void* params)
{
	Location mouseLocation;
	g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
	CellCoord mouseCoord = util::locationToCellCoord(mouseLocation);

	if (eventType == GAME_SYSTEM_EVENT_KEYDOWN)
	{
		int key = (int)params;
		if (key == HGEK_LBUTTON)
		{
			m_bMouseTracking = true;			

			if (mouseCoord == game.m_startCoord)
			{
				m_brush = Brush_Start;
			}
			else if (mouseCoord == game.m_targetCoord)
			{
				m_brush = Brush_Target;
			}
			else
			{
				if (g_Board.isBlock(mouseCoord))
				{
					g_Board.setBlock(mouseCoord, false);
					m_brush = Brush_Erase;
				}
				else
				{
					g_Board.setBlock(mouseCoord, true);
					m_brush = Brush_Block;
				}				
			}
			m_lastMouseCoord = mouseCoord;
		}
	}
	else if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		if (key == HGEK_LBUTTON)
		{
			m_bMouseTracking = false;
			if (m_brush == Brush_Start || m_brush == Brush_Target)
			{
				if (g_Board.isBlock(mouseCoord))
				{
					g_Board.setBlock(mouseCoord, false);
				}
			}
		}		
	}
}

void GameModeMapEdit::update(Game& game, float delta)
{
	if (!m_bMouseTracking)
	{
		return;
	}
	
	Location mouseLocation;
	g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
	CellCoord mouseCoord = util::locationToCellCoord(mouseLocation);
	if (mouseCoord == m_lastMouseCoord)
	{
		return;
	}
	m_lastMouseCoord = mouseCoord;
	
	if (m_brush == Brush_Block)
	{
		if (mouseCoord == game.m_startCoord || mouseCoord == game.m_targetCoord || g_Board.isBlock(mouseCoord) )
		{
			return;
		}
		else
		{
			g_Board.setBlock(mouseCoord, true);
		}
	}
	else if (m_brush == Brush_Erase)
	{
		if (mouseCoord == game.m_startCoord || mouseCoord == game.m_targetCoord || !g_Board.isBlock(mouseCoord) )
		{
			return;
		}
		else
		{
			g_Board.setBlock(mouseCoord, false);
		}
	}
	else if (m_brush == Brush_Start)
	{
		if (mouseCoord == game.m_targetCoord)
		{
			return;
		}
		game.m_startCoord = mouseCoord;
	}
	else if (m_brush == Brush_Target)
	{
		if (mouseCoord == game.m_startCoord)
		{
			return;
		}
		game.m_targetCoord = mouseCoord;
	}
}
