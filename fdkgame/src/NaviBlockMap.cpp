#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviTileMap.h>

namespace fdk { namespace game { namespace navi
{
	BlockMap::BlockMap(TileMap& tileMap, MoveCapability moveCapability)
		: m_tileMap(tileMap)
		, m_moveCapability(moveCapability)
	{
	}

	BlockMap::~BlockMap()
	{
	}
	
	void BlockMap::rebuildFromTileMap()
	{
		m_data.reset(m_tileMap.getSizeX(), m_tileMap.getSizeY());
		for (size_t y = 0; y < m_data.size_y(); ++y)
		{
			for (size_t x = 0; x < m_data.size_y(); ++x)
			{
				if (!fdk::hasEnumMask(m_moveCapability, m_tileMap.getTileType(CellCoord(x, y)) ) )
				{
					m_data(x, y) = true;
				}				
			}
		}
	}

	size_t BlockMap::getSizeX() const
	{
		return m_data.size_x();
	}

	size_t BlockMap::getSizeY() const
	{
		return m_data.size_y();
	}

	MoveCapability BlockMap::getMoveCapability() const
	{
		return m_moveCapability;
	}

	bool BlockMap::isBlock(const CellCoord& cellCoord) const
	{
		return m_data(cellCoord.x, cellCoord.y);
	}

}}}