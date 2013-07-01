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
	
	struct VertexMapType
	{
		MoveCapability moveCapability;
		UnitSize unitSize;
		VertexMapType(MoveCapability moveCapability, UnitSize unitSize);
		bool operator<(const VertexMapType& other) const;
	};
	
	inline VertexMapType::VertexMapType(MoveCapability l_moveCapability, UnitSize l_unitSize)
		: moveCapability(l_moveCapability)
		, unitSize(l_unitSize)
	{
	}

	inline bool VertexMapType::operator<(const VertexMapType& other) const
	{
		if (moveCapability != other.moveCapability)
		{
			return moveCapability < other.moveCapability;
		}
		return unitSize < other.unitSize;
	}

	inline bool checkMoveCapabilityOnTileType(MoveCapability capability, TileType tileType)
	{
		return !((capability^tileType) & tileType);
	}
}}}

#endif
