#ifndef __FDKGAME_NAVIMAPMANAGER_H_INCLUDE__
#define __FDKGAME_NAVIMAPMANAGER_H_INCLUDE__
#include <vector>
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
		struct PlotUnitArgument
		{
			PlotUnitArgument();
			VertexCoord vertexCoord;
			UnitSize unitSize;	
		};
		struct AutoPlotUnits
		{
			AutoPlotUnits(MapManager& mapManager, const std::vector<PlotUnitArgument>& units, MoveCapability* moveCapability);
			~AutoPlotUnits();
		private:
			MapManager& m_mapManager;
			bool m_bMoveCapability;
			MoveCapability m_moveCapability;
			std::vector<PlotUnitArgument> m_units;
		};
		struct VertexMapType
		{
			MoveCapability moveCapability;
			UnitSize unitSize;
			VertexMapType(MoveCapability moveCapability, UnitSize unitSize);
			bool operator<(const VertexMapType& other) const;
		};
		MapManager(TileMap& tileMap, 
			const std::set<MoveCapability>& moveCapabilities, 
			UnitSize minUnitSize, UnitSize maxUnitSize);
		~MapManager();
		const BlockMap& getBlockMap(MoveCapability moveCapability) const;
		const VertexMap& getVertexMap(MoveCapability moveCapability, UnitSize unitSize) const;		
		void rebuildAfterTileMapReset();
		void changeTileType(const CellCoord& cellCoord, TileType tileType);
		void increExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count=1);
		void decreExtraTileType(const CellCoord& cellCoord, TileType tileType, ExtraTileCountType count=1);
		void plotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot, MoveCapability* moveCapability=0);
		void allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow);
	private:
		struct CmpBlockMap
		{
			bool operator()(const BlockMap* a, const BlockMap* b) const;
		};
		typedef std::set<BlockMap*, CmpBlockMap> BlockMaps;
		typedef std::map<VertexMapType, VertexMap*> VertexMaps;		
		const BlockMap* findBlockMap(MoveCapability moveCapability) const;
		const VertexMap* findVertexMap(MoveCapability moveCapability, UnitSize unitSize) const;
		void onTileChange(const CellCoord& cellCoord);
		TileMap& m_tileMap;
		BlockMaps m_blockMaps; // key by unit size
		VertexMaps m_vertexMaps;
		bool m_allowVertexBlockValueMinus;
	};	

	inline MapManager::PlotUnitArgument::PlotUnitArgument()
		: vertexCoord()
		, unitSize(-1)
	{
	}

	inline MapManager::VertexMapType::VertexMapType(MoveCapability l_moveCapability, UnitSize l_unitSize)
		: moveCapability(l_moveCapability)
		, unitSize(l_unitSize)
	{
	}

	inline bool MapManager::VertexMapType::operator<(const VertexMapType& other) const
	{
		if (moveCapability != other.moveCapability)
		{
			return moveCapability < other.moveCapability;
		}
		return unitSize < other.unitSize;
	}
}}}

#endif
