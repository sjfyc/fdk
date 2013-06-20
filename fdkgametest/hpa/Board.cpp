#include "Board.h"
#include "Util.h"
#include "Font.h"
#include "Option.h"
#pragma warning(disable:4244)

Board::Board()
{
	_Base::resetMap(CELL_COUNT_X, CELL_COUNT_Y, g_Option.getUnitSize());
	annotateMap();
}

Board::~Board()
{
}

bool Board::isValidCoord(const CellCoord& coord) const
{
	return _Base::isValidNodeCoord(coord);
}

void Board::clearBlocks()
{
	_Base::clearObstacles();
}

void Board::setBlock(const CellCoord& coord, bool bBlock)
{
	_Base::setObstacle(getNodeID(coord), bBlock);
}

bool Board::isBlock(const CellCoord& coord) const
{
	return _Base::isObstacle(getNodeID(coord));
}

void Board::draw()
{
	for (int i = 0; i <= CELL_COUNT_Y; ++i) // ºáÏß
	{	
		g_HGE->Gfx_RenderLine(0, i*CELL_SIZE_Y, CELL_COUNT_X*CELL_SIZE_X, i*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (int i = 0; i <= CELL_COUNT_X; ++i) // ÊúÏß
	{
		g_HGE->Gfx_RenderLine(i*CELL_SIZE_X, 0, i*CELL_SIZE_X, CELL_COUNT_Y*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (short x = 0; x < CELL_COUNT_X; ++x)
	{
		for (short y = 0; y < CELL_COUNT_Y; ++y)
		{
			if (isBlock(CellCoord(x, y)) )
			{
				util::fillCell(CellCoord(x, y), COLOR_CELL_BLOCK);
			}
		}
	}
}

void Board::drawCellCoord()
{
	for (int y = 0; y < CELL_COUNT_Y; ++y)
	{	
		for (int x = 0; x < CELL_COUNT_X; ++x)
		{
			g_Font.printf(x*CELL_SIZE_X+2, y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d,%d", x, y);
		}
	}
}

void Board::drawCellClearanceValue()
{
	for (int y = 0; y < CELL_COUNT_Y; ++y)
	{	
		for (int x = 0; x < CELL_COUNT_X; ++x)
		{
			g_Font.printf(x*CELL_SIZE_X+2, y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d", getClearanceValue(getNodeID(CellCoord(x, y)) ) );
		}
	}
}
