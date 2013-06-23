#ifndef __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#define __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"
#include "NaviBase.h"

namespace fdk { namespace game { namespace navi
{
	class BlockMap;	

	// 对应某种尺寸单位的顶点地图：每个顶点存储对于该尺寸单位的碰撞数量
	class FDKGAME_API VertexMap
		: public Environment
	{
	public:
		VertexMap(BlockMap& blockMap, UnitSize unitSize);
		~VertexMap();		
		void rebuildFromBlockMap();		
		
		size_t getSizeX() const;
		size_t getSizeY() const;

		VertexID toVertexID(const VertexCoord& vertexCoord) const;
		VertexCoord toVertexCoord(VertexID vertexID) const;		
		bool isValidVertexID(VertexID vertexID) const;
		bool isValidVertexCoord(const VertexCoord& vertexCoord) const;

		int getBlockValue(const VertexCoord& vertexCoord) const;
		bool isBlock(const VertexCoord& vertexCoord) const;
		void onSetBlock(const CellCoord& cellCoord, bool bSet);

		void onPlotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool xAlign, bool yAlign, bool bPlot);

		bool isDirectlyReachable(const VertexCoord& startVertexCoord, const VertexCoord& targetVertexCoord) const;

		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
	private:
		typedef Array2D<int> MapData;
		bool tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const VertexCoord& vertexCoord, int cost, int parentNodeID) const;
		BlockMap& m_blockMap;
		UnitSize m_unitSize;
		MapData m_data;
	};
}}}

#endif
