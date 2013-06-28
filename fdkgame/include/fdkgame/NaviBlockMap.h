#ifndef __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#define __FDKGAME_NAVIBLOCKMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class TileMap;

	// 对应某一种移动能力的障碍地图：每个格子只存储是否为障碍
	class FDKGAME_API BlockMap
	{
		friend class MapManager;
	public:
		BlockMap(TileMap& tileMap, MoveCapability moveCapability);
		~BlockMap();
		void rebuildFromTileMap();		
		size_t getSizeX() const;
		size_t getSizeY() const;
		MoveCapability getMoveCapability() const;
		void setBlock(const CellCoord& cellCoord, bool bSet=true);
		bool isBlock(const CellCoord& cellCoord) const;
	private:
		bool checkCapabilityOnTile(const CellCoord& cellCoord) const;
		TileMap& m_tileMap;
		MoveCapability m_moveCapability;
		Array2D<bool> m_data;
	};

	inline BlockMap::BlockMap(TileMap& tileMap, MoveCapability moveCapability)
		: m_tileMap(tileMap)
		, m_moveCapability(moveCapability)
	{
	}

	inline BlockMap::~BlockMap()
	{
	}

	inline size_t BlockMap::getSizeX() const
	{
		return m_data.size_x();
	}

	inline size_t BlockMap::getSizeY() const
	{
		return m_data.size_y();
	}

	inline MoveCapability BlockMap::getMoveCapability() const
	{
		return m_moveCapability;
	}

	inline void BlockMap::setBlock(const CellCoord& cellCoord, bool bSet)
	{
		m_data(cellCoord.x, cellCoord.y) = bSet;
	}

	inline bool BlockMap::isBlock(const CellCoord& cellCoord) const
	{
		return m_data(cellCoord.x, cellCoord.y);
	}
}}}

#endif
