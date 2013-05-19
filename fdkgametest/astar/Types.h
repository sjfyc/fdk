#ifndef __GAMETYPES_H_INCLUDE__
#define __GAMETYPES_H_INCLUDE__
#include <fdkgame/Base.h>
#include <common/hgeall.h>

typedef fdkgame::Coord<short> CellCoord;

enum 
{
	CELL_COUNT_X = 26,
	CELL_COUNT_Y = 20,

	CELL_SIZE_X = 40,
	CELL_SIZE_Y = 40,

	COLOR_BOARD_LINE = 0x55020202,
	COLOR_CELL_BLOCK = 0xFF222222,
	COLOR_CELL_FROM = 0xFF00FF00,
	COLOR_CELL_TO = 0xFFFF0000,

	COLOR_BLACK = 0xFF000000,
	COLOR_WHITE = 0xFFFFFFFF,
};

#endif
