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
				VertexMapType vertexMapType(blockMap->getMoveCapability(), unitSize);
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

	const VertexMap& MapManager::getVertexMap(const VertexMapType& vertexMapType) const
	{
		return *findVertexMap(vertexMapType);
	}

	VertexMap& MapManager::getVertexMapForOperate(const VertexMapType& vertexMapType)
	{
		return *findVertexMap(vertexMapType);
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

	const VertexMap* MapManager::findVertexMap(const VertexMapType& vertexMapType) const
	{
		VertexMaps::const_iterator it = m_vertexMaps.find(vertexMapType);
		if (it == m_vertexMaps.end())
		{
			return 0;
		}
		return it->second;
	}

	VertexMap* MapManager::findVertexMap(const VertexMapType& vertexMapType)
	{
		return const_cast<VertexMap*>(
			static_cast<const MapManager*>(this)->findVertexMap(vertexMapType)
			);
	}

	void MapManager::changeTileType(const CellCoord& cellCoord, TileType tileType, bool bModBuildedWall)
	{
		if (m_tileMap.getTileType(cellCoord) == tileType)
		{
			return;
		}
		m_tileMap.setTileType(cellCoord, tileType);
		
		onTileChange(cellCoord, bModBuildedWall);
	}

	void MapManager::increExtraTileType(const CellCoord& cellCoord, TileType tileType, bool bModBuildedWall, ExtraTileCountType count)
	{
		m_tileMap.increExtraTileType(cellCoord, tileType, count);
		onTileChange(cellCoord, bModBuildedWall);
	}

	void MapManager::decreExtraTileType(const CellCoord& cellCoord, TileType tileType, bool bModBuildedWall, ExtraTileCountType count)
	{
		m_tileMap.decreExtraTileType(cellCoord, tileType, count);
		onTileChange(cellCoord, bModBuildedWall);
	}

	void MapManager::plotUnit(const VertexMapType& vertexMapType, const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot)
	{
		getVertexMapForOperate(vertexMapType).onPlotUnit(vertexCoord, unitSize, bPlot);
	}

	void MapManager::allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow)
	{
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMap* vertexMap = it->second;
			vertexMap->allowModify(vertexCoord, unitSize, bAllow);
		}
	}

	void MapManager::onTileChange(const CellCoord& cellCoord, bool bModBuildedWall)
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
					if (vertexMapType.moveCapability != blockMap->getMoveCapability())
					{
						continue;
					}
					vertexMap->onSetBlock(cellCoord, bSet, bModBuildedWall);
				}
			}
		}
	}

	void MapManager::rebuildColors()
	{
		for (VertexMaps::iterator it = m_vertexMaps.begin(); it != m_vertexMaps.end(); ++it)
		{
			VertexMap* vertexMap = it->second;
			vertexMap->getColorComponent()->refill();
		}
	}

	MapManager::AutoPlotUnits::AutoPlotUnits(
		MapManager& mapManager, 
		const VertexMapType& vertexMapType, 
		const std::vector<PlotUnitArgument>& units
		)
		: m_mapManager(mapManager)
		, m_vertexMapType(vertexMapType)
		, m_units(units)
	{
		if (!m_units.empty())
		{			
			for (size_t i = 0; i < m_units.size(); ++i)
			{
				const PlotUnitArgument& arg = m_units[i];
				m_mapManager.plotUnit(m_vertexMapType, arg.vertexCoord, arg.unitSize, true);
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
				m_mapManager.plotUnit(m_vertexMapType, arg.vertexCoord, arg.unitSize, false);
			}
		}
	}
}}}
