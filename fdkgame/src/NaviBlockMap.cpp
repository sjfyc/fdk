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
				if (!fdk::hasEnumMask(m_moveCapability, m_tileMap.getTileType(CellCoord(x, y)) ) )
				{
					m_data(x, y) = true;
				}				
			}
		}
	}
	
}}}
