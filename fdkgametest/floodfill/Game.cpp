#include "Game.h"
#include <iostream>
#include <fdk/Module.h>
#include "Util.h"
#include "Option.h"
#include "Board.h"

Game::Game()
	: m_mode(&g_GameModeMapEdit)
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
	if (g_Option.isOn(Option::Toggle_ShowCellCoord))
	{
		g_Board.drawCellCoord();
	}	

	float x0 = util::cellCoordToLocation(m_startCoord).x;
	float y0 = util::cellCoordToLocation(m_startCoord).y;
	float x1 = util::cellCoordToLocation(m_targetCoord).x;
	float y1 = util::cellCoordToLocation(m_targetCoord).y;
	CellCoord offset = m_targetCoord-m_startCoord;
	
	util::fillCell(m_startCoord, COLOR_CELL_FROM);
	util::fillCell(m_targetCoord, COLOR_CELL_TO);

	if (g_Option.isOn(Option::Toggle_ShowCenterVertex))
	{
		g_Board.drawCenterVertex();
	}	

	if (g_Option.isOn(Option::Toggle_ShowConnectLine) )
	{
		g_HGE->Gfx_RenderLine(x0+HALF_CELL_SIZE_X,y0+HALF_CELL_SIZE_Y,x1+HALF_CELL_SIZE_X,y1+HALF_CELL_SIZE_Y,MyColor_Blue);

		if (offset.x > 0 && offset.y < 0)
		{
			g_HGE->Gfx_RenderLine(x0,y0,x1,y1,MyColor_Blue);
			g_HGE->Gfx_RenderLine(x0+CELL_SIZE_X,y0+CELL_SIZE_Y,x1+CELL_SIZE_X,y1+CELL_SIZE_Y,MyColor_Blue);
		}
		else if (offset.x > 0 && offset.y > 0)
		{
			g_HGE->Gfx_RenderLine(x0+CELL_SIZE_X,y0,x1+CELL_SIZE_X,y1,MyColor_Blue);
			g_HGE->Gfx_RenderLine(x0,y0+CELL_SIZE_Y,x1,y1+CELL_SIZE_Y,MyColor_Blue);
		}
		else if (offset.x < 0 && offset.y < 0)
		{
			g_HGE->Gfx_RenderLine(x1+CELL_SIZE_X,y1,x0+CELL_SIZE_X,y0,MyColor_Blue);
			g_HGE->Gfx_RenderLine(x1,y1+CELL_SIZE_Y,x0,y0+CELL_SIZE_Y,MyColor_Blue);
		}
		else if (offset.x < 0 && offset.y > 0)
		{
			g_HGE->Gfx_RenderLine(x1,y1,x0,y0,MyColor_Blue);
			g_HGE->Gfx_RenderLine(x1+CELL_SIZE_X,y1+CELL_SIZE_Y,x0+CELL_SIZE_X,y0+CELL_SIZE_Y,MyColor_Blue);
		}
	}
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
	}
	m_mode->handleEvent(*this, eventType, params);
}

void Game::outputUsage()
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
}

void GameModeMapEdit::leave(Game& game)
{
	g_Board.annotateMap();
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
		
	if (m_brush == Brush_Erase)
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
