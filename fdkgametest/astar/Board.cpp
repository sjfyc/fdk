#include "Board.h"
#include "Util.h"
#pragma warning(disable:4244)
Board::Board()
{
	fdk::zeroArray(m_block);
}

Board::~Board()
{
}

bool Board::isValidCoord(const CellCoord& coord) const
{
	return coord.x >= 0 && coord.x < CELL_COUNT_X
		&& coord.y >= 0 && coord.y < CELL_COUNT_Y;
}

void Board::setBlock(const CellCoord& coord, bool bBlock)
{
	FDK_ASSERT(isValidCoord(coord));
	m_block[coord.x][coord.y] = bBlock;
}

bool Board::isBlock(const CellCoord& coord) const
{
	if (!isValidCoord(coord))
	{
		return false;
	}
	return m_block[coord.x][coord.y];
}

void Board::draw()
{
	for (int i = 0; i <= CELL_COUNT_Y; ++i) // ����
	{	
		g_HGE->Gfx_RenderLine(0, i*CELL_SIZE_Y, CELL_COUNT_X*CELL_SIZE_X, i*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (int i = 0; i <= CELL_COUNT_X; ++i) // ����
	{
		g_HGE->Gfx_RenderLine(i*CELL_SIZE_X, 0, i*CELL_SIZE_X, CELL_COUNT_Y*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (short x = 0; x < CELL_COUNT_X; ++x)
	{
		for (short y = 0; y < CELL_COUNT_Y; ++y)
		{
			if (m_block[x][y])
			{
				util::fillCell(CellCoord(x, y), COLOR_CELL_BLOCK);
			}
		}
	}
}
