#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Option.h"
#include "TileMap.h"
#include "ActorBank.h"

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
}

void Game::render()
{
	g_TileMap.draw();
	g_TileMap.drawCellInfo();
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
	//util::output("SPACE: restart/continue path finding");
	//util::output("S: restart/continue path finding step by step");
}

bool Game::isInGameMode() const
{
	return m_mode == &g_GameModeGame;
}

GameModeGame::GameModeGame()
{
}

void GameModeGame::update(Game& game, float delta)
{
}

void GameModeGame::render(Game& game)
{
	g_ActorBank.draw();
}

void GameModeGame::handleEvent(Game& game, int eventType, void* params)
{
}

GameModeMapEdit::GameModeMapEdit()
	: m_bMouseTracking(false)
	, m_lastMouseCoord()
{
}

void GameModeMapEdit::enter(Game& game)
{
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
