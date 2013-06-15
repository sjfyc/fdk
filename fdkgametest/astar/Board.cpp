#include "Board.h"
#include "Util.h"
#pragma warning(disable:4244)

Board::Board()
{
	_Base::resetMap(CELL_COUNT_X, CELL_COUNT_Y);
}

Board::~Board()
{
}

bool Board::isValidCoord(const CellCoord& coord) const
{
	return _Base::isValidNodeCoord(coord);
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
