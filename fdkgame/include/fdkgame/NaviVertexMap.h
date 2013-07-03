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
		: public GridEnv
	{
		typedef GridEnv _Base;
	public:
		VertexMap(BlockMap& blockMap, UnitSize unitSize);
		~VertexMap();		
		void rebuildFromBlockMap();		
		
		int getBlockValue(const VertexCoord& vertexCoord) const;
		bool isBlock(const VertexCoord& vertexCoord) const;
		void onSetBlock(const CellCoord& cellCoord, bool bSet);

		void onPlotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot);
		void allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow);

		// GridEnv interfaces
		virtual int getSizeX() const;
		virtual int getSizeY() const;		

		// Environment interfaces
		virtual bool isNodeReachable(int nodeID) const;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
	private:
		typedef Array2D<int> MapData;
		BlockMap& m_blockMap;
		UnitSize m_unitSize;
		MapData m_data;
		Array2D<bool> m_allowModify;
	};
	
	inline int VertexMap::getBlockValue(const VertexCoord& vertexCoord) const
	{
		return m_data(vertexCoord.x, vertexCoord.y);
	}

	inline bool VertexMap::isBlock(const VertexCoord& vertexCoord) const
	{
		return getBlockValue(vertexCoord) > 0;
	}
}}}

#endif
