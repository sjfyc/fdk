#include "MapManager.h"
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Font.h"
#include "Option.h"
#include "TileMap.h"
#include "Actor.h"
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
	: _Base(g_TileMap, getSupportedMoveCapabilities(), MIN_UNIT_SIZE, MAX_UNIT_SIZE)
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
		for (int y = 0; y < vertexMap.getSizeY(); ++y)
		{
			for (int x = 0; x < vertexMap.getSizeX(); ++x)
			{
				util::drawVertex(VertexCoord(x, y), 
					(vertexMap.getBlockValue(VertexCoord(x, y)) > 0) ? ARGB(255, 226, 98, 29) : ARGB(255, 131, 164, 108)
					);
			}
		}
	}	

	if (g_Option.isOn(Option::Toggle_ShowVertexCoordInMouse))
	{
		Location mouseLocation;
		g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);

		VertexCoord vertexCoord = util::locationToVertexCoord(mouseLocation);

		g_Font.printf(mouseLocation.x-20.0f,mouseLocation.y-20.0f,HGETEXT_LEFT,"%d,%d", vertexCoord.x, vertexCoord.y);
	}	
}

bool MapManager::isLocationReachable(Actor& actor, const Location& location)
{
	const fdkgame::navi::VertexMap& vertexMap = getVertexMap(actor.getMoveCapability(), actor.getUnitSize());
	return !vertexMap.isBlock(util::locationToVertexCoord(location));
}
