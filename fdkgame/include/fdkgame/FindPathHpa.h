#ifndef __FDKGAME_FINDPATHHPA_H_INCLUDE__
#define __FDKGAME_FINDPATHHPA_H_INCLUDE__
#include "FindPathBase.h"
#include "FindPathGridMap.h"
#include "Graph.h"

namespace fdk { namespace game { namespace findpath
{
	class FDKGAME_API AbstractGridMap
		: public Environment
	{
	public:
		struct AbstractNodeInfo
		{
			int lowLevelNodeID;
		};
		struct AbstractEdgeInfo
		{			
			int cost;
		};
		typedef Graph<AbstractNodeInfo, AbstractEdgeInfo> AbstractGraph;
		typedef GraphNode<AbstractNodeInfo, AbstractEdgeInfo> AbstractNode;
		typedef GraphEdge<AbstractNodeInfo, AbstractEdgeInfo> AbstractEdge;
		typedef VectorI ClusterCoord;
		class Cluster;
		struct Entrance
		{
			int lowLevelNode1ID;
			int lowLevelNode2ID;
			Cluster* cluster1;
			Cluster* cluster2;
		};
		class Cluster
			: public GridMapPart
		{
			friend class AbstractGridMap;
		public:
			const ClusterCoord& getClusterCoord() const;
		private:
			Cluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord);
			ClusterCoord m_clusterCoord;
			std::vector<AbstractNode*> m_entrances;
		};

		AbstractGridMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void rebuildAbstract();
		//
		const VectorI& getMaxClusterSize() const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const { return m_abstractGraph.getNodes().size(); }
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	protected:
		void clear();
		void createClusterAndEntrances();
		void buildAbstractGraph();
		void createHorizontalEntrances(int xStart, int xEnd, int y, Cluster& cluster2);
		void createVerticalEntrances(int yStart, int yEnd, int x, Cluster& cluster2);
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		Array2D<Cluster*> m_clusters;
		std::vector<Entrance> m_entrances;
		AbstractGraph m_abstractGraph;
	};

	inline AbstractGridMap::Cluster::Cluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord)
		: GridMapPart(orignMap, range)
		, m_clusterCoord(clusterCoord)
	{
	}

	inline const AbstractGridMap::ClusterCoord& AbstractGridMap::Cluster::getClusterCoord() const
	{
		return m_clusterCoord;
	}

	inline const VectorI& AbstractGridMap::getMaxClusterSize() const
	{
		return m_maxClusterSize;
	}

}}}

#endif
