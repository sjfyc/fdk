#include "MapManager.h"
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Font.h"
#include "Option.h"
#include "TileMap.h"
#pragma warning(disable:4244)
namespace
{
	std::set<fdkgame::navi::MoveCapability> getSupportedMoveCapabilities()
	{
		std::set<fdkgame::navi::MoveCapability> result;
		fdkgame::navi::MoveCapability moveCapability = 0;
		fdk::setEnumMask(moveCapability, Tile_None);
		result.insert(moveCapability);

		moveCapability = 0;
		fdk::setEnumMask(moveCapability, Tile_None);
		fdk::setEnumMask(moveCapability, Tile_Water);
		result.insert(moveCapability);

		return result;
	}
}

MapManager::MapManager()
	: _Base(g_TileMap, getSupportedMoveCapabilities(), 1, 2)
{
	rebuildAfterTileMapReset();
}

MapManager::~MapManager()
{
}

void MapManager::draw()
{
	const fdkgame::navi::VertexMap& vertexMap = getVertexMap(g_Option.getMoveCapability(), g_Option.getUnitSize());
	for (size_t y = 0; y < vertexMap.getSizeY(); ++y)
	{
		for (size_t x = 0; x < vertexMap.getSizeX(); ++x)
		{
			g_HGE.FrameRect(
				(x+1)*CELL_SIZE_X/2-2, 
				(y+1)*CELL_SIZE_Y/2-2, 
				(x+1)*CELL_SIZE_X/2+2, 
				(y+1)*CELL_SIZE_Y/2+2, 
				(vertexMap.getBlockValue(CellCoord(x, y)) > 0) ? ARGB(255, 226, 98, 29) : MyColor_Green
				);
		}
	}
}
