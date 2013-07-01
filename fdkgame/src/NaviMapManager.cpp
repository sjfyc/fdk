#include <fdkgame/NaviMapManager.h>
#include <fdkgame/NaviTileMap.h>
#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviVertexMap.h>

namespace fdk { namespace game { namespace navi
{
	bool MapManager::CmpBlockMap::operator()(const BlockMap* a, const BlockMap* b) const
	{
		return a->getMoveCapability() < b->getMoveCapability();
	}

	MapManager::MapManager(TileMap& tileMap, const std::set<MoveCapability>& moveCapabilities, UnitSize minUnitSize, UnitSize maxUnitSize)
		: m_tileMap(tileMap)
	{
		FDK_ASSERT(minUnitSize >= 1 && maxUnitSize >= minUnitSize);
	
		for (std::set<MoveCapability>::const_iterator it = moveCapabilities.begin(); 
			it != moveCapabilities.end(); ++it)
		{
			const MoveCapability moveCapability = *it;
			BlockMap* blockMap = new BlockMap(m_tileMap, moveCapability);
			bool result = m_blockMaps.insert(blockMap).second;
			FDK_ASSERT(result);
		}

		for (BlockMaps::iterator it = m_blockMaps.begin(); it != m_blockMaps.end(); ++it)
		{
			BlockMap* blockMap = *it;		
			for (UnitSize unitSize = minUnitSize; unitSize <= maxUnitSize; ++unitSize)
			{
				VertexMap* vertexMap = new VertexMap(*blockMap, unitSize);
				VertexMapType vertexMapType = std::make_pair(blockMap->getMoveCapability(), unitSize);
				bool result = m_vertexMaps.insert(std::make_pair(vertexMapType, vertexMap)).second;
				FDK_ASSERT(result);
			}
		}		
	}

	MapManager::~MapManager()
	{
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMap* vertexMap = it->second;
			delete vertexMap;
		}
		for (BlockMaps::iterator it = m_blockMaps.begin(); it != m_blockMaps.end(); ++it)
		{
			BlockMap* blockMap = *it;
			delete blockMap;
		}
	}

	void MapManager::rebuildAfterTileMapReset()
	{
		for (BlockMaps::iterator it = m_blockMaps.begin(); it != m_blockMaps.end(); ++it)
		{
			BlockMap* blockMap = *it;
			blockMap->rebuildFromTileMap();
		}
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMap* vertexMap = it->second;
			vertexMap->rebuildFromBlockMap();
		}
	}

	const BlockMap& MapManager::getBlockMap(MoveCapability moveCapability) const
	{
		return *findBlockMap(moveCapability); 
	}

	const VertexMap& MapManager::getVertexMap(MoveCapability moveCapability, UnitSize unitSize) const
	{
		return *findVertexMap(moveCapability, unitSize);
	}

	const BlockMap* MapManager::findBlockMap(MoveCapability moveCapability) const
	{
		FDK_CMP_PTR(BlockMap, search);
		search->m_moveCapability = moveCapability;
		BlockMaps::const_iterator it = m_blockMaps.find(search);
		if (it == m_blockMaps.end())
		{
			return 0;
		}
		return *it;
	}

	const VertexMap* MapManager::findVertexMap(MoveCapability moveCapability, UnitSize unitSize) const
	{
		VertexMaps::const_iterator it = m_vertexMaps.find(std::make_pair(moveCapability, unitSize) );
		if (it == m_vertexMaps.end())
		{
			return 0;
		}
		return it->second;
	}

	void MapManager::changeTileType(const CellCoord& cellCoord, TileType tileType)
	{
		if (m_tileMap.getTileType(cellCoord) == tileType)
		{
			return;
		}
		m_tileMap.setTileType(cellCoord, tileType);
		
		onTileChange(cellCoord);
	}

	void MapManager::increExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count)
	{
		m_tileMap.increExtraTileType(cellCoord, tileType, count);
		onTileChange(cellCoord);
	}

	void MapManager::decreExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count)
	{
		m_tileMap.decreExtraTileType(cellCoord, tileType, count);
		onTileChange(cellCoord);
	}

	void MapManager::plotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot, MoveCapability* moveCapability)
	{
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMapType vertexMapType = it->first;
			VertexMap* vertexMap = it->second;
			if (moveCapability && vertexMapType.first != *moveCapability)
			{
				continue;
			}
			vertexMap->onPlotUnit(vertexCoord, unitSize, bPlot);
		}
	}

	void MapManager::allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow)
	{
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMap* vertexMap = it->second;
			vertexMap->allowModify(vertexCoord, unitSize, bAllow);
		}
	}

	void MapManager::onTileChange(const CellCoord& cellCoord)
	{
		for (BlockMaps::iterator it = m_blockMaps.begin(); it != m_blockMaps.end(); ++it)
		{
			BlockMap* blockMap = *it;
			bool bChange = false;
			bool bSet = false;			
			if (blockMap->isBlock(cellCoord) && blockMap->checkCapabilityOnTile(cellCoord))				
			{// 之前不可走，现在可走
				blockMap->setBlock(cellCoord, false);
				bChange = true;
				bSet = false;
			}
			else if (!blockMap->isBlock(cellCoord) && !blockMap->checkCapabilityOnTile(cellCoord))
			{// 之前可走，现在不可走
				blockMap->setBlock(cellCoord, true);
				bChange = true;
				bSet = true;
			}
			if (bChange)
			{
				for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
				{
					VertexMapType vertexMapType = it->first;
					VertexMap* vertexMap = it->second;
					if (vertexMapType.first != blockMap->getMoveCapability())
					{
						continue;
					}
					vertexMap->onSetBlock(cellCoord, bSet);
				}
			}
		}
	}

	MapManager::AutoPlotUnits::AutoPlotUnits(
		MapManager& mapManager, 
		const std::vector<PlotUnitArgument>& units,
		MoveCapability* moveCapability, 
		const PlotUnitArgument* subtract, 
		bool bPolt)
		: m_mapManager(mapManager)
		, m_bMoveCapability(moveCapability ? true : false)
		, m_moveCapability(moveCapability ? *moveCapability : 0)
		, m_units(units)
		, m_bPlot(bPolt)
		, m_subtractUnit()
	{
		if (!m_units.empty())
		{
			if (subtract)
			{
				m_subtractUnit = *subtract;
			}
			if (m_subtractUnit.unitSize > 0)
			{
				m_mapManager.allowModify(m_subtractUnit.vertexCoord, m_subtractUnit.unitSize, false);
			}
			for (size_t i = 0; i < m_units.size(); ++i)
			{
				const PlotUnitArgument& arg = m_units[i];
				m_mapManager.plotUnit(arg.vertexCoord, arg.unitSize, m_bPlot, m_bMoveCapability ? &m_moveCapability : 0);
			}
		}		
	}

	MapManager::AutoPlotUnits::~AutoPlotUnits()
	{
		if (!m_units.empty())
		{
			for (size_t i = 0; i < m_units.size(); ++i)
			{
				const PlotUnitArgument& arg = m_units[i];
				m_mapManager.plotUnit(arg.vertexCoord, arg.unitSize, !m_bPlot, m_bMoveCapability ? &m_moveCapability : 0);
			}
			if (m_subtractUnit.unitSize > 0)
			{
				m_mapManager.allowModify(m_subtractUnit.vertexCoord, m_subtractUnit.unitSize, true);
			}
		}		
	}
}}}
