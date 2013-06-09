#include "Game.h"
#include "Util.h"
#include "Board.h"
#include "ActorBank.h"

bool Game::start()
{
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);

	m_coordFrom.reset(3, CELL_COUNT_Y/2);
	m_coordTo.reset(CELL_COUNT_X-4, CELL_COUNT_Y/2);	
	return true;
}

void Game::stop()
{
	fdk::EventHook::unregist();
}

void Game::update(float delta)
{
	g_ActorBank.update(delta);
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
	if (eventType == GAME_SYSTEM_EVENT_KEYDOWN)
	{
		int key = (int)params;
		std::cout << key << " down" << std::endl;
	}
	else if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		std::cout << key << " up" << std::endl;		
	}
}