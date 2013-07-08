#ifndef __GAMETYPES_H_INCLUDE__
#define __GAMETYPES_H_INCLUDE__
#include <common/hgeall.h>
#include <fdkgame/Math.h>
#include <fdkgame/FindPathAStar.h>

typedef fdkgame::Vector2D<short> CellCoord;
typedef fdkgame::Vector2D<float> Location;
typedef fdkgame::Vector2D<float> Velocity;

enum 
{
	COLOR_BLACK = 0xFF000000,
	COLOR_WHITE = 0xFFFFFFFF,
	COLOR_YELLOW = 0xFFFFFF00,

	CELL_COUNT_X = 46,
	CELL_COUNT_Y = 30,

	CELL_SIZE_X = 32,
	CELL_SIZE_Y = 32,

	HALF_CELL_SIZE_X = CELL_SIZE_X/2,
	HALF_CELL_SIZE_Y = CELL_SIZE_Y/2,

	QUATER_CELL_SIZE_X = CELL_SIZE_X/4,
	QUATER_CELL_SIZE_Y = CELL_SIZE_Y/4,

	COLOR_BOARD_LINE = 0x55020202,
	COLOR_CELL_BLOCK = 0xFF222222,
	COLOR_CELL_FROM = MyColor_Green,
	COLOR_CELL_TO = MyColor_Red,	
};

#endif
