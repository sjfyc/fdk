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
	if (g_Option.isOn(Option::Toggle_ShowVertex))
	{
		const fdkgame::navi::VertexMap& vertexMap = getVertexMap(g_Option.getMoveCapability(), g_Option.getUnitSize());
		for (size_t y = 0; y < vertexMap.getSizeY(); ++y)
		{
			for (size_t x = 0; x < vertexMap.getSizeX(); ++x)
			{
				Location location = util::vertexCoordToLocation(VertexCoord(x, y));
				g_HGE.FrameRect(
					location.x-2, 
					location.y-2, 
					location.x+2, 
					location.y+2, 
					(vertexMap.getBlockValue(VertexCoord(x, y)) > 0) ? ARGB(255, 226, 98, 29) : MyColor_Green
					);
			}
		}
	}	
}
