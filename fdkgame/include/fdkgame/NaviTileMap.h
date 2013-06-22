#ifndef __FDKGAME_NAVITILEMAP_H_INCLUDE__
#define __FDKGAME_NAVITILEMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	typedef unsigned char TileType;

	// 格子地图：每个格子只存储地形信息
	class FDKGAME_API TileMap
	{
	public:		
		void reset(size_t sizeX, size_t sizeY, TileType tileType);
		size_t getSizeX() const;
		size_t getSizeY() const;

		void setTileType(const CellCoord& cellCoord, TileType tileType);
		TileType getTileType(const CellCoord& cellCoord) const;
		
		bool isValidCellCoord(const CellCoord& cellCoord) const;

		CellID toCellID(const CellCoord& cellCoord) const;
		CellCoord toCellCoord(CellID cellID) const;
	private:
		Array2D<TileType> m_data;
	};
}}}

#endif
