#ifndef __GAMETYPES_H_INCLUDE__
#define __GAMETYPES_H_INCLUDE__
#include <common/hgeall.h>
#include <fdkgame/Math.h>

typedef fdkgame::Vector2D<short> CellCoord;
typedef fdkgame::Vector2D<int> VertexCoord;
typedef fdkgame::Vector2D<float> Location;

typedef fdkgame::Rect<float> BoundingBox;
typedef fdkgame::Rect<short> CellRange;
typedef fdkgame::Rect<float> LocationRange;

typedef fdkgame::Vector2D<float> Velocity;

enum 
{
	CELL_COUNT_X = 40,
	CELL_COUNT_Y = 20,

	CELL_SIZE_X = 30,
	CELL_SIZE_Y = 30,

	HALF_CELL_SIZE_X = CELL_SIZE_X/2,
	HALF_CELL_SIZE_Y = CELL_SIZE_Y/2,

	MIN_UNIT_SIZE  = 1,
	MAX_UNIT_SIZE  = 3, 
};

enum 
{
	Event_OnActorSelected = GAME_USER_EVENT,
};

enum EunmTile
{
	Tile_None,
	Tile_Block,
	Tile_Water,
	Tile__Count,
};
typedef unsigned char TileType;
inline DWORD getTileColor(TileType tile)
{
	DWORD colors[] =
	{
		MyColor_White,
		0xFF222222,
		ARGB(255, 0, 128, 255),
	};
	return colors[tile];
}

inline const char* getTileName(TileType tile)
{
	const char* names[] =
	{
		"none",
		"block",
		"water",
	};
	return names[tile];
}

#endif
