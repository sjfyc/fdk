#ifndef __FDKGAME_FINDPATHHPA_H_INCLUDE__
#define __FDKGAME_FINDPATHHPA_H_INCLUDE__
#include "FindPathBase.h"
#include "FindPathGridMap.h"
#include "Graph.h"

namespace fdk { namespace game { namespace findpath
{
	class AbstractGridMap
		: public Environment
	{
	public:
		struct AbsNodeInfo
		{
			int lowLevelNodeID;
		};
		struct AbsEdgeInfo
		{			
			int cost;
		};
		struct Entrance
		{
			int node1ID;
			int node2ID;
		};
		class Cluster
		{
			friend class AbstractGridMap;
		public:
			const VectorI& getClusterCoord() const;
			const VectorI& getSize() const;
		private:
			Cluster(const VectorI& clusterCoord, const VectorI& size);
			VectorI m_clusterCoord;
			VectorI m_size;
			std::vector<int> m_absEntranceNodes;
		};

		AbstractGridMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void buildAbstract();
		//
		const VectorI& getMaxClusterSize() const;
		// Environment interfaces
		virtual int getNodeCount() const { return m_absGraph.getNodes().size(); }
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	protected:
		typedef Graph<AbsNodeInfo, AbsEdgeInfo> AbsGraph;
		typedef GraphNode<AbsNodeInfo, AbsEdgeInfo> AbsNode;
		typedef GraphEdge<AbsNodeInfo, AbsEdgeInfo> AbsEdge;
		void createHorizontalEntrances(int xStart, int xEnd, int y);
		void createVerticalEntrances(int yStart, int yEnd, int x);
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		Array2D<Cluster*> m_clusters;
		std::vector<Entrance> m_entrances;
		AbsGraph m_absGraph;
	};

	inline AbstractGridMap::Cluster::Cluster(const VectorI& clusterCoord, const VectorI& size)
		: m_clusterCoord(clusterCoord)
		, m_size(size)
	{
	}

	inline const VectorI& AbstractGridMap::Cluster::getClusterCoord() const
	{
		return m_clusterCoord;
	}

	inline const VectorI& AbstractGridMap::Cluster::getSize() const
	{
		return m_size;
	}	

	inline const VectorI& AbstractGridMap::getMaxClusterSize() const
	{
		return m_maxClusterSize;
	}

}}}

#endif
