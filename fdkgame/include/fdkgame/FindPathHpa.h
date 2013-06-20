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
			friend class Hpa;
		public:
			typedef std::vector<AbstractNode*> Ports;
			const ClusterCoord& getClusterCoord() const;
			const Ports& getPorts() const;			
		private:
			AbstractNode* findPortWithLowLevelNodeID(int lowLevelNodeID) const;
			void localToLowLevelPath(const std::vector<int>& local, std::vector<int>& lowLevel) const;
			Cluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord);
			ClusterCoord m_clusterCoord;
			Ports m_ports;
		};

		HpaMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void clearAbstract();
		void rebuildAbstract(); // 需要在lowLevelMap重新annotateMap(比如布局发生了变化)或者改变了minClearanceValueRequired后调用

		GridMap& getLowLevelMap() const;
		const VectorI& getMaxClusterSize() const;
		const Array2D<Cluster*>& getClusters() const;
		const AbstractGraph& getAbstractGraph() const;

		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	protected:		
		void createClusterAndBridges();
		void buildAbstractGraph();
		void buildAbstractGraph_addIntraEdgesInCluster(Cluster& cluster);
		void createVerticalBridges(int xStart, int xEnd, int y, Cluster& cluster2);
		void createHorizontalBridges(int yStart, int yEnd, int x, Cluster& cluster2);
		std::pair<AbstractNode*, bool> addStartOrTargetNodeForHpa(int lowLevelNodeID, bool bStart);
		Cluster& getClusterOfLowLevelNode(int lowLevelNodeID) const;
		void abstractToLowLevelPath(const std::vector<int>& abstract, std::vector<int>& lowLevel) const;
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		Array2D<Cluster*> m_clusters;		
		AbstractGraph m_abstractGraph;
		std::vector<Bridge> m_bridges;
	};

	class Hpa
	{
	public:
		enum ErrorType
		{			
			Error_OK,
			Error_PathCompleted,
			Error_PathUnexist,
		};
		Hpa(HpaMap& env, int startNodeID, int targetNodeID, int minClearanceValueRequired=1);
		~Hpa();		
		int popNextPathNode();
		ErrorType getError() const;
		const std::vector<int>& getRoughPath() const;
	protected:
		void initSearch();
		HpaMap& m_env;
		int m_lowLevelStartNodeID;
		int m_lowLevelTargetNodeID;
		int m_minClearanceValueRequired;
		ErrorType m_error;
		std::vector<int> m_roughPath;
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

	inline const HpaMap::Cluster::Ports& HpaMap::Cluster::getPorts() const
	{
		return m_ports;
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

	inline Hpa::ErrorType Hpa::getError() const
	{
		return m_error;
	}

	inline const std::vector<int>& Hpa::getRoughPath() const
	{
		return m_roughPath;
	}
}}}

#endif
