#include <fdkgame/NaviTileMap.h>

namespace fdk { namespace game { namespace navi
{
	void TileMap::reset(size_t sizeX, size_t sizeY, TileType tileType)
	{
		m_data.reset(sizeX, sizeY, tileType);
		m_extraData.reset(sizeX, sizeY);
	}

	TileMap::ExtraTileCounter* TileMap::ExtraTileData::findCounter(TileType tileType)
	{
		for (size_t i = 0; i < tileCounters.size(); ++i)
		{
			ExtraTileCounter& extraTileCounter= tileCounters[i];
			if (extraTileCounter.tileType == tileType)
			{
				return &extraTileCounter;
			}
		}
		return 0;
	}

	void TileMap::increExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count)
	{
		ExtraTileData& extraData = m_extraData(cellCoord.x, cellCoord.y);
		ExtraTileCounter* extraTileCounter = extraData.findCounter(tileType);
		if (extraTileCounter)
		{
			extraTileCounter->count += count;
		}
		else
		{
			ExtraTileCounter newCounter;
			newCounter.tileType = tileType;
			newCounter.count = count;
			extraData.tileCounters.push_back(newCounter);
		}
	}

	void TileMap::decreExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count)
	{
		ExtraTileData& extraData = m_extraData(cellCoord.x, cellCoord.y);
		ExtraTileCounter* extraTileCounter = extraData.findCounter(tileType);
		if (extraTileCounter)
		{
			FDK_ASSERT(extraTileCounter->count >= count);
			extraTileCounter->count -= count;
		}
		else
		{
			FDK_ASSERT(0);
		}
	}	
}}}
