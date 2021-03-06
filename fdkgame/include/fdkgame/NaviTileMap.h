#ifndef __FDKGAME_NAVITILEMAP_H_INCLUDE__
#define __FDKGAME_NAVITILEMAP_H_INCLUDE__
#include <vector>
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	// 格子地图：每个格子只存储地形信息
	class FDKGAME_API TileMap
	{
	public:
		struct ExtraTileCounter
		{
			TileType tileType;
			ExtraTileCountType count;
		};
		struct ExtraTileData
		{
			ExtraTileCounter* findCounter(TileType tileType);
			std::vector<ExtraTileCounter> tileCounters;
		};

		void reset(size_t sizeX, size_t sizeY, TileType tileType);
		size_t getSizeX() const;
		size_t getSizeY() const;

		void setTileType(const CellCoord& cellCoord, TileType tileType);
		TileType getTileType(const CellCoord& cellCoord) const;

		void increExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count=1);
		void decreExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count=1);
		const ExtraTileData& getExtraTileData(const CellCoord& cellCoord) const;
		
		bool isValidCellCoord(const CellCoord& cellCoord) const;

		CellID toCellID(const CellCoord& cellCoord) const;
		CellCoord toCellCoord(CellID cellID) const;
	private:
		Array2D<TileType> m_data;
		Array2D<ExtraTileData> m_extraData;
	};

	inline size_t TileMap::getSizeX() const
	{
		return m_data.size_x();
	}

	inline size_t TileMap::getSizeY() const
	{
		return m_data.size_y();
	}

	inline void TileMap::setTileType(const CellCoord& cellCoord, TileType tileType)
	{
		m_data(cellCoord.x, cellCoord.y) = tileType;
	}

	inline TileType TileMap::getTileType(const CellCoord& cellCoord) const
	{
		return m_data(cellCoord.x, cellCoord.y);
	}

	inline bool TileMap::isValidCellCoord(const CellCoord& cellCoord) const
	{
		return m_data.is_valid_index(cellCoord.x, cellCoord.y);
	}

	inline CellID TileMap::toCellID(const CellCoord& cellCoord) const
	{
		return cellCoord.y * m_data.size_x() + cellCoord.x;
	}

	inline CellCoord TileMap::toCellCoord(CellID cellID) const
	{
		return CellCoord(cellID % m_data.size_x(), cellID / m_data.size_x());
	}

	inline const TileMap::ExtraTileData& TileMap::getExtraTileData(const CellCoord& cellCoord) const
	{
		return m_extraData(cellCoord.x, cellCoord.y);
	}
}}}

#endif
