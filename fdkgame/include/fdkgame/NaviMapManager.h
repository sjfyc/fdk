#ifndef __FDKGAME_NAVIMAPMANAGER_H_INCLUDE__
#define __FDKGAME_NAVIMAPMANAGER_H_INCLUDE__
#include <set>
#include <map>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class TileMap;
	class BlockMap;
	class VertexMap;
	
	class FDKGAME_API MapManager
	{
	public:
		MapManager(TileMap& tileMap, 
			const std::set<MoveCapability>& moveCapabilities, 
			UnitSize minUnitSize, UnitSize maxUnitSize);
		~MapManager();
		const VertexMap& getVertexMap(MoveCapability moveCapability, UnitSize unitSize) const;
		void changeTileType(const CellCoord& cellCoord, TileType tileType);
		void rebuildAfterTileMapReset();
	private:
		struct CmpBlockMap
		{
			bool operator()(const BlockMap* a, const BlockMap* b) const;
		};
		typedef std::set<BlockMap*, CmpBlockMap> BlockMaps;
		typedef std::pair<MoveCapability, UnitSize> VertexMapType;
		typedef std::map<VertexMapType, VertexMap*> VertexMaps;		
		const VertexMap* findVertexMap(MoveCapability moveCapability, UnitSize unitSize) const;
		TileMap& m_tileMap;
		BlockMaps m_blockMaps; // key by unit size
		VertexMaps m_vertexMaps;
	};
}}}

#endif
