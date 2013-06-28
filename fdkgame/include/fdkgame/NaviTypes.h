#ifndef __FDKGAME_NAVITYPES_H_INCLUDE__
#define __FDKGAME_NAVITYPES_H_INCLUDE__
#include "Math.h"

namespace fdk { namespace game { namespace navi
{	
	typedef int TileType;
	typedef unsigned short ExtraTileCountType;
	typedef Vector2D<int> CellCoord;	
	typedef int CellID;
	typedef Vector2D<int> VertexCoord;
	typedef int VertexID;
	typedef int MoveCapability;
	typedef int UnitSize;
	typedef std::pair<MoveCapability, UnitSize> VertexMapType;
	static const int COST_STRAIGHT = 100;	
	static const int COST_DIAGONAL = 142;
	
	inline bool checkMoveCapabilityOnTileType(MoveCapability capability, TileType tileType)
	{
		return !((capability^tileType) & tileType);
	}
}}}

#endif
