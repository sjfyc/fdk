#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Option.h"
#include "Board.h"
#include "HpaMap.h"
#include "ActorBank.h"

Game::Game()
	: m_mode(&g_GameModeGame)
	, m_hpa(0)
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
	g_Option.start();

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
	g_HpaMap.draw();
	g_Board.drawCellInfo();
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
		else if (key == HGEK_T)
		{
			toggleMode();
		}
		else if (key == HGEK_SPACE || key == HGEK_S)
		{
			if (m_mode == &g_GameModeMapEdit)
			{
				toggleMode();
			}
			if (!m_hpa || m_hpa->getError() != Hpa::Error_OK)
			{				
				delete m_hpa;
				m_hpa = new Hpa(0, g_HpaMap, 
					g_Board.toNodeID(m_startCoord),
					g_Board.toNodeID(m_targetCoord));
				if (m_hpa->getError() == Hpa::Error_PathUnexist)
				{
					util::output("path unexist");
				}
			}
		}
	}
	m_mode->handleEvent(*this, eventType, params);
}

void Game::toggleMode()
{
	m_mode->leave(*this);
	if (m_mode == &g_GameModeGame)
	{
		m_mode = &g_GameModeMapEdit;
	}
	else 
	{
		m_mode = &g_GameModeGame;
	}
	m_mode->enter(*this);
	util::output("toggle to %s mode", (m_mode == &g_GameModeGame ? "game" : "map edit") );
}

void Game::outputUsage()
{
	util::output("T: toggle between game & map edit mode");
	util::output("C: clear obstacles in map edit mode");
	util::output("SPACE: restart/continue path finding");
	util::output("S: restart/continue path finding step by step");
}

GameModeGame::GameModeGame()
	: m_bStepByStep(false)
{
}

void GameModeGame::update(Game& game, float delta)
{
	Hpa* hpa = game.m_hpa;
	if (hpa && hpa->getError() == Hpa::Error_OK && !m_bStepByStep)
	{
		search(*hpa);
	}	
}

void GameModeGame::render(Game& game)
{
	if (game.m_hpa)
	{
		game.m_hpa->render();
	}
	g_ActorBank.draw();
}

void GameModeGame::handleEvent(Game& game, int eventType, void* params)
{
	if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;		
		if (key == HGEK_SPACE)
		{
			m_bStepByStep = false;
		}
		else if (key == HGEK_S)
		{
			m_bStepByStep = true;
			search(*game.m_hpa);
		}
	}
}

void GameModeGame::search(Hpa& hpa)
{
	int node = hpa.popNextPathNode();
	if (node == fdkgame::findpath::INVALID_NODEID)
	{
		if (hpa.getError() == Hpa::Error_PathCompleted)
		{
			util::output("target reached");
		}
		else
		{
			util::output("path unexist");
		}
		return;
	}
}

GameModeMapEdit::GameModeMapEdit()
	: m_bMouseTracking(false)
	, m_lastMouseCoord()
	, m_brush(Brush_Block)
{
}

void GameModeMapEdit::enter(Game& game)
{
	FDK_DELETE(game.m_hpa);
	g_HpaMap.clearAbstract();
}

void GameModeMapEdit::leave(Game& game)
{
	g_Board.annotateMap();
	g_HpaMap.rebuildAbstract();
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
		else if (key == HGEK_C)
		{
			g_Board.clearBlocks();
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
