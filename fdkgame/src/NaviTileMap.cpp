#include <fdkgame/NaviTileMap.h>

namespace fdk { namespace game { namespace navi
{
	void TileMap::reset(size_t sizeX, size_t sizeY, TileType tileType)
	{
		m_data.reset(sizeX, sizeY, tileType);
	}

	size_t TileMap::getSizeX() const
	{
		return m_data.size_x();
	}

	size_t TileMap::getSizeY() const
	{
		return m_data.size_y();
	}

	void TileMap::setTileType(const CellCoord& cellCoord, TileType tileType)
	{
		m_data(cellCoord.x, cellCoord.y) = tileType;
	}

	TileType TileMap::getTileType(const CellCoord& cellCoord) const
	{
		return m_data(cellCoord.x, cellCoord.y);
	}

	bool TileMap::isValidCellCoord(const CellCoord& cellCoord) const
	{
		return m_data.is_valid_index(cellCoord.x, cellCoord.y);
	}

	CellID TileMap::toCellID(const CellCoord& cellCoord) const
	{
		return cellCoord.y * m_data.size_x() + cellCoord.x;
	}

	CellCoord TileMap::toCellCoord(CellID cellID) const
	{
		return CellCoord(cellID % m_data.size_x(), cellID / m_data.size_x());
	}

	
}}}
