#ifndef __GAMETYPES_H_INCLUDE__
#define __GAMETYPES_H_INCLUDE__
#include <common/hgeall.h>
#include <fdkgame/Math.h>

typedef fdkgame::Vector2D<short> CellCoord;
typedef fdkgame::Vector2D<float> Location;
typedef fdkgame::Vector2D<float> Velocity;
typedef fdkgame::Rect<float> BoundingBox;

enum 
{
	CELL_COUNT_X = 26,
	CELL_COUNT_Y = 20,

	CELL_SIZE_X = 20,
	CELL_SIZE_Y = 20,
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
