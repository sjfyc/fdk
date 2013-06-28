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
			AutoPlotUnits(MapManager& mapManager, const std::vector<PlotUnitArgument>& units, const PlotUnitArgument* subtract=0, bool bPolt=true);
			~AutoPlotUnits();
		private:
			MapManager& m_mapManager;
			std::vector<PlotUnitArgument> m_units;
			bool m_bPlot;
			PlotUnitArgument m_subtractUnit;
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
		void plotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot);
		void allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow);
	private:
		struct CmpBlockMap
		{
			bool operator()(const BlockMap* a, const BlockMap* b) const;
		};
		typedef std::set<BlockMap*, CmpBlockMap> BlockMaps;
		typedef std::pair<MoveCapability, UnitSize> VertexMapType;
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
}}}

#endif
