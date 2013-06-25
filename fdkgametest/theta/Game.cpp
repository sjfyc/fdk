#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Option.h"
#include "Font.h"
#include "TileMap.h"
#include "MapManager.h"
#include "ActorBank.h"

Game::Game()
	: m_mode(&g_GameModeGame)
	, m_savedDeltaTime(0)
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
	
	return true;
}

void Game::stop()
{
	fdk::EventHook::unregist();

	fdk::ModuleManager::instance().stop();
}

void Game::update(float delta)
{	
	if (!fdk::ModuleManager::instance().tick(delta))
	{
		util::output(fdk::ModuleManager::instance().getErrorMessage().c_str());
	}
	m_mode->update(*this, delta);

	m_savedDeltaTime = delta;
}

void Game::render()
{
	g_TileMap.draw();
	g_MapManager.draw();
		
	m_mode->render(*this);	
	g_ActorBank.draw();
	g_TileMap.drawCellInfo();

	//g_Font.printf(200,200,HGETEXT_LEFT,"%f",m_savedDeltaTime);
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
		else if (key == HGEK_P)
		{
			pauseGame();
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
}

void Game::outputUsage()
{
	util::output("T: toggle between game & map edit mode");
	util::output("C: clear obstacles in map edit mode");
	util::output("D: dynamic change tile type in game mode");
	//util::output("SPACE: restart/continue path finding");
	//util::output("S: restart/continue path finding step by step");
}

bool Game::isInGameMode() const
{
	return m_mode == &g_GameModeGame;
}

void Game::pauseGame()
{
	GameMode* nextMode = &g_GameModePause;
	if (m_mode == &g_GameModePause)
	{
		nextMode = g_GameModePause.m_lastMode;
	}
	else
	{
		g_GameModePause.m_lastMode = m_mode;
	}
	m_mode->leave(*this);		
	m_mode = nextMode;			
	m_mode->enter(*this);
}

GameModeGame::GameModeGame()
	: m_canDynamicChangeTile(false)
{
}

void GameModeGame::enter(Game& game)
{
	util::output("toggle to game mode");
}

void GameModeGame::leave(Game& game)
{

}

void GameModeGame::update(Game& game, float delta)
{
}

void GameModeGame::render(Game& game)
{	
}

void GameModeGame::handleEvent(Game& game, int eventType, void* params)
{
	if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		if (key == HGEK_D)
		{
			m_canDynamicChangeTile = !m_canDynamicChangeTile;
		}
	}

	Location mouseLocation;
	g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
	CellCoord mouseCoord = util::locationToCellCoord(mouseLocation);

	if (eventType == GAME_SYSTEM_EVENT_KEYDOWN)
	{
		int key = (int)params;
		if (key == HGEK_LBUTTON)
		{
			if (m_canDynamicChangeTile)
			{
				g_MapManager.changeTileType(mouseCoord, g_Option.getBrush());
			}			
		}
	}
}

GameModeMapEdit::GameModeMapEdit()
	: m_bMouseTracking(false)
	, m_lastMouseCoord()
{
}

void GameModeMapEdit::enter(Game& game)
{
	util::output("toggle to map edit mode");
}

void GameModeMapEdit::leave(Game& game)
{
	g_MapManager.rebuildAfterTileMapReset();
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

			g_TileMap.setTileType(mouseCoord, g_Option.getBrush());
			m_lastMouseCoord = mouseCoord;
		}
	}
	else if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		if (key == HGEK_LBUTTON)
		{
			m_bMouseTracking = false;			
		}
		else if (key == HGEK_C)
		{
			g_TileMap.reset(g_TileMap.getSizeX(), g_TileMap.getSizeY(), Tile_None);
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

	g_TileMap.setTileType(mouseCoord, g_Option.getBrush());
}

void GameModePause::enter(Game& game)
{
	util::output("toggle to pause mode");
}

void GameModePause::leave(Game& game)
{

}
