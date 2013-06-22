#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Option.h"
#include "TileMap.h"

Game::Game()
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
}

void Game::render()
{
	g_TileMap.draw();
	g_TileMap.drawCellInfo();
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
	}
}

void Game::outputUsage()
{
	//util::output("T: toggle between game & map edit mode");
	//util::output("C: clear obstacles in map edit mode");
	//util::output("SPACE: restart/continue path finding");
	//util::output("S: restart/continue path finding step by step");
}
