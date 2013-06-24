#ifndef __FDKGAME_NAVIGRIDMAP_H_INCLUDE__
#define __FDKGAME_NAVIGRIDMAP_H_INCLUDE__
#include "NaviBase.h"
#include "NaviTypes.h"
#include <fdk/Array2D.h>
#include "Math.h"

namespace fdk { namespace game { namespace navi
{
	class FDKGAME_API GridMap
		: public GridBasedEnv
	{
	public:		
		struct Node
		{
			Node();
			int clearanceValue; // -1代表未初始化，0代表障碍，>=1代表正常值
		};
		typedef Array2D<Node> MapData;
		static const int CLEARANCEVALUE_UNINITED = -1;
		static const int CLEARANCEVALUE_OBSTACLE = 0;
		GridMap();
		void resetMap(size_t sizeX, size_t sizeY);
		void clearObstacles();
		void setObstacle(int nodeID, bool bSet=true);
		void annotateMap();
		
		const MapData& getMapData() const;

		void setMinClearanceValueRequired(int minClearanceValueRequired);
		int getMinClearanceValueRequired() const;
		bool meetMinClearanceValueRequired(int nodeID) const;
		int getClearanceValue(int nodeID) const;				
	
		// GridBasedEnv interfaces
		virtual int getSizeX() const;
		virtual int getSizeY() const;
		virtual bool isNodeReachable(int nodeID) const;

		// Environment interfaces
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
	private:		
		void annotateNode(const NodeCoord& coord);
		bool tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& coord, int cost, int parentNodeID) const;
		MapData m_nodes;
		int m_minClearanceValueRequired;
	};
	
	inline GridMap::Node::Node()
		: clearanceValue(CLEARANCEVALUE_UNINITED)
	{
	}

	inline GridMap::GridMap()
	{
		m_minClearanceValueRequired = 1;
	}

	inline void GridMap::resetMap(size_t sizeX, size_t sizeY)
	{
		m_nodes.reset(sizeX, sizeY);
	}

	inline const GridMap::MapData& GridMap::getMapData() const
	{
		return m_nodes;
	}
	
	inline void GridMap::setMinClearanceValueRequired(int minClearanceValueRequired)
	{
		m_minClearanceValueRequired = minClearanceValueRequired;
	}

	inline int GridMap::getMinClearanceValueRequired() const
	{
		return m_minClearanceValueRequired;
	}
	
	inline bool GridMap::meetMinClearanceValueRequired(int nodeID) const
	{
		return getClearanceValue(nodeID) >= m_minClearanceValueRequired;
	}

	inline void GridMap::setObstacle(int nodeID, bool bSet)
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		m_nodes.raw_data()[nodeID].clearanceValue = bSet ? CLEARANCEVALUE_OBSTACLE : CLEARANCEVALUE_UNINITED;
	}
	
	inline int GridMap::getClearanceValue(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return m_nodes.raw_data()[nodeID].clearanceValue;
	}
	
}}}

#endif
