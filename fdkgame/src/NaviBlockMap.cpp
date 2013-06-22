#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviTileMap.h>

namespace fdk { namespace game { namespace navi
{
	BlockMap::BlockMap(int moveCapability)
		: m_moveCapability(moveCapability)
	{
	}

	BlockMap::~BlockMap()
	{
	}
	
	void BlockMap::rebuildFromTileMap(const TileMap& tileMap)
	{
		m_data.reset(tileMap.getSizeX(), tileMap.getSizeX());
		for (size_t y = 0; y < tileMap.getSizeY(); ++y)
		{
			for (size_t x = 0; x < tileMap.getSizeX(); ++x)
			{
				if (!fdk::hasEnumMask(m_moveCapability, tileMap.getTileType(CellCoord(x, y)) ) )
				{
					m_data(x, y) = true;
				}				
			}
		}
	}
}}}
