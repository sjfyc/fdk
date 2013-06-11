#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Board.h"
#include "ActorBank.h"

bool Game::start()
{
	if (!fdk::ModuleManager::instance().start())
	{
		std::cout << fdk::ModuleManager::instance().getErrorMessage() << std::endl;
		return false;
	}
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);

	m_coordFrom.reset(3, CELL_COUNT_Y/2);
	m_coordTo.reset(CELL_COUNT_X-4, CELL_COUNT_Y/2);	
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
		std::cout << fdk::ModuleManager::instance().getErrorMessage() << std::endl;		
	}
}

void Game::render()
{
	g_Board.draw();
	util::fillCell(m_coordFrom, COLOR_CELL_FROM);
	util::fillCell(m_coordTo, COLOR_CELL_TO);
	g_ActorBank.draw();
	g_HGE.DrawMouse();
}
#include <iostream>
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