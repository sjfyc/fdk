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
	
		for (std::set<MoveCapability>::iterator it = moveCapabilities.begin(); 
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

	const VertexMap& MapManager::getVertexMap(MoveCapability moveCapability, UnitSize unitSize) const
	{
		return *findVertexMap(moveCapability, unitSize);
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
}}}
