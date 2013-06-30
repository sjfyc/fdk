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
	const fdkgame::navi::VertexMap& vertexMap = getVertexMap(g_Option.getMoveCapability(), g_Option.getUnitSize());

	if (g_Option.isOn(Option::Toggle_ShowVertex))
	{		
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

		VertexCoord vertexCoord = util::locationToNearestVertexCoord(mouseLocation);

		g_Font.printf(mouseLocation.x-20.0f,mouseLocation.y-20.0f,HGETEXT_LEFT,"%d,%d", vertexCoord.x, vertexCoord.y);
	}	
	if (g_Option.isOn(Option::Toggle_ShowVertexIDInMouse))
	{
		Location mouseLocation;
		g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);

		VertexCoord vertexCoord = util::locationToNearestVertexCoord(mouseLocation);

		if (vertexMap.isValidNodeCoord(vertexCoord))
		{
			g_Font.printf(mouseLocation.x-20.0f,mouseLocation.y-20.0f,HGETEXT_LEFT,"%d", vertexMap.toNodeID(vertexCoord) );
		}		
	}
}

bool MapManager::isLocationReachable(Actor& actor, const Location& location)
{
	const fdkgame::navi::VertexMap& vertexMap = getVertexMap(actor.getMoveCapability(), actor.getUnitSize());
	VertexCoord testVertexCoord = util::locationToNearestVertexCoord(location);
	if (!vertexMap.isBlock(testVertexCoord) )
	{
		return true;
	}

	//const Velocity& velocity = actor.getVelocity();
	//if ((velocity.x > 0 && velocity.y > 0) || // 右下
	//	(velocity.x < 0 && velocity.y < 0) ) // 左上
	//{
	//	if (!vertexMap.isBlock(testVertexCoord+VertexCoord(1, -1)) || // 右上角可走
	//		!vertexMap.isBlock(testVertexCoord+VertexCoord(-1, 1)) ) // 左下角可走
	//	{
	//		return true;
	//	}
	//}
	//else if ((velocity.x > 0 && velocity.y < 0) || // 右上
	//	(velocity.x < 0 && velocity.y > 0) ) // 左下
	//{
	//	if (!vertexMap.isBlock(testVertexCoord+VertexCoord(1, 1)) || // 右下角可走
	//		!vertexMap.isBlock(testVertexCoord+VertexCoord(-1, -1)) ) // 左上角可走
	//	{
	//		return true;
	//	}
	//}
	return false;
}
