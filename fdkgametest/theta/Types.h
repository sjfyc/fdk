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
	//CELL_COUNT_X = 200,
	//CELL_COUNT_Y = 140,

	//CELL_SIZE_X = 8,
	//CELL_SIZE_Y = 8,

	CELL_COUNT_X = 46,
	CELL_COUNT_Y = 32,

	CELL_SIZE_X = 32,
	CELL_SIZE_Y = 32,

	HALF_CELL_SIZE_X = CELL_SIZE_X/2,
	HALF_CELL_SIZE_Y = CELL_SIZE_Y/2,

	QUATER_CELL_SIZE_X = CELL_SIZE_X/4,
	QUATER_CELL_SIZE_Y = CELL_SIZE_Y/4,

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
	if (tile == (1<<Tile_None))
	{
		return MyColor_White;
	}
	else if (tile == (1<<Tile_Block))
	{
		return 0xFF222222;
	}
	else if (tile == (1<<Tile_Water))
	{
		return ARGB(255, 0, 128, 255);
	}
	else 
	{
		return 0;
	}

	//DWORD colors[] =
	//{
	//	MyColor_White,
	//	0xFF222222,
	//	ARGB(255, 0, 128, 255),
	//};
	//return colors[tile];
}

inline const char* getTileName(TileType tile)
{
	if (tile == (1<<Tile_None))
	{
		return "none";
	}
	else if (tile == (1<<Tile_Block))
	{
		return "block";
	}
	else if (tile == (1<<Tile_Water))
	{
		return "water";
	}
	else 
	{
		return "";
	}

	//const char* names[] =
	//{
	//	"none",
	//	"block",
	//	"water",
	//};
	//return names[tile];
}

inline int getPlotActorRadiusInVertex()
{
	return 30*MAX_UNIT_SIZE*2;
}

inline float getPlotActorRadius()
{
	return (float)getPlotActorRadiusInVertex()*HALF_CELL_SIZE_X;
}


#endif
