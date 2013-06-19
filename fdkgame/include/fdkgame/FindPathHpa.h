#ifndef __FDKGAME_FINDPATHHPA_H_INCLUDE__
#define __FDKGAME_FINDPATHHPA_H_INCLUDE__
#include "FindPathBase.h"
#include "FindPathGridMap.h"
#include "Graph.h"

namespace fdk { namespace game { namespace findpath
{
	class FDKGAME_API HpaMap
		: public Environment
	{
		friend class Hpa;
	public:
		struct AbstractNodeInfo
		{
			int lowLevelNodeID;
		};
		struct AbstractEdgeInfo
		{
			bool bIntra;
			int cost;
		};
		typedef Graph<AbstractNodeInfo, AbstractEdgeInfo> AbstractGraph;
		typedef GraphNode<AbstractNodeInfo, AbstractEdgeInfo> AbstractNode;
		typedef GraphEdge<AbstractNodeInfo, AbstractEdgeInfo> AbstractEdge;
		typedef VectorI ClusterCoord;
		class Cluster;
		struct Bridge
		{
			int lowLevelNode1ID;
			int lowLevelNode2ID;
			Cluster* cluster1;
			Cluster* cluster2;
		};
		class Cluster
			: public GridMapPart
		{
			friend class HpaMap;
		public:
			typedef std::vector<AbstractNode*> Entrances;
			const ClusterCoord& getClusterCoord() const;
			const Entrances& getEntrances() const;
			AbstractNode* findEntranceWithLowLevelNodeID(int lowLevelNodeID) const;
		private:
			Cluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord);
			ClusterCoord m_clusterCoord;
			Entrances m_entrances;
		};

		HpaMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void rebuildAbstract();

		GridMap& getLowLevelMap() const;
		const VectorI& getMaxClusterSize() const;
		const Array2D<Cluster*>& getClusters() const;
		const AbstractGraph& getAbstractGraph() const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	protected:
		void clear();
		void createClusterAndBridges();
		void buildAbstractGraph();
		void buildAbstractGraph_addIntraEdgesInCluster(Cluster& cluster);
		void createVerticalBridges(int xStart, int xEnd, int y, Cluster& cluster2);
		void createHorizontalBridges(int yStart, int yEnd, int x, Cluster& cluster2);
		std::pair<AbstractNode*, bool> addStartOrTargetNodeAfterBuildedAbstract(int lowLevelNodeID, bool bStart);
		Cluster& getClusterOfLowLevelNode(int lowLevelNodeID) const;
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		Array2D<Cluster*> m_clusters;		
		AbstractGraph m_abstractGraph;
		std::vector<Bridge> m_bridges;
	};

	class Hpa
	{
	public:
		enum SearchResult
		{			
			SearchResult_Proceeding,
			SearchResult_Completed,
			SearchResult_PathUnexist,
		};
		Hpa(HpaMap& env, int startNodeID, int targetNodeID);
		~Hpa();
		int popNextPathNode();
	private:
		void initSearch();		
		HpaMap& m_env;
		int m_startNodeID;
		int m_targetNodeID;
		std::vector<HpaMap::AbstractNode*> m_tempAddedStartTarget;
		SearchResult m_searchResult;
		std::vector<int> m_path;
		std::vector<int> m_localRefinedPath;
		int m_pathCost;
	};

	inline HpaMap::Cluster::Cluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord)
		: GridMapPart(orignMap, range)
		, m_clusterCoord(clusterCoord)
	{
	}

	inline const HpaMap::ClusterCoord& HpaMap::Cluster::getClusterCoord() const
	{
		return m_clusterCoord;
	}

	inline const HpaMap::Cluster::Entrances& HpaMap::Cluster::getEntrances() const
	{
		return m_entrances;
	}

	inline GridMap& HpaMap::getLowLevelMap() const
	{
		return m_lowLevelMap;
	}

	inline const VectorI& HpaMap::getMaxClusterSize() const
	{
		return m_maxClusterSize;
	}

	inline const Array2D<HpaMap::Cluster*>& HpaMap::getClusters() const
	{
		return m_clusters;
	}

	inline const HpaMap::AbstractGraph& HpaMap::getAbstractGraph() const
	{
		return m_abstractGraph;
	}
}}}

#endif
