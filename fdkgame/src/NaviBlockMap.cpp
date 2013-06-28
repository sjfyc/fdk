#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviTileMap.h>

namespace fdk { namespace game { namespace navi
{
	void BlockMap::rebuildFromTileMap()
	{
		m_data.reset(m_tileMap.getSizeX(), m_tileMap.getSizeY());
		for (size_t y = 0; y < m_data.size_y(); ++y)
		{
			for (size_t x = 0; x < m_data.size_x(); ++x)
			{
				if (!checkCapabilityOnTile(CellCoord(x, y) ) )
				{
					m_data(x, y) = true;
				}				
			}
		}
	}

	bool BlockMap::checkCapabilityOnTile(const CellCoord& cellCoord) const
	{
		if (!checkMoveCapabilityOnTileType(m_moveCapability, m_tileMap.getTileType(cellCoord) ) )
		{
			return false;
		}
		const TileMap::ExtraTileData& extraTileData = m_tileMap.getExtraTileData(cellCoord);
		for (size_t i = 0; i < extraTileData.tileCounters.size(); ++i)
		{
			const TileMap::ExtraTileCounter& extraTileCounter = extraTileData.tileCounters[i];
			if (extraTileCounter.count > 0 && !checkMoveCapabilityOnTileType(m_moveCapability, extraTileCounter.tileType) )
			{
				return false;
			}
		}
		return true;
	}
}}}
