#ifndef __FDKGAME_FINDPATHHPA_H_INCLUDE__
#define __FDKGAME_FINDPATHHPA_H_INCLUDE__
#include "FindPathBase.h"
#include "FindPathGridMap.h"
#include "Graph.h"

namespace fdk { namespace game { namespace findpath
{
	class HpaCluster;

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
		struct Bridge
		{
			int lowLevelNode1ID;
			int lowLevelNode2ID;
			HpaCluster* cluster1;
			HpaCluster* cluster2;
		};
		typedef VectorI ClusterCoord;
		HpaMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void clearAbstract();
		void rebuildAbstract(); // 需要在lowLevelMap重新annotateMap(比如布局发生了变化)或者改变了minClearanceValueRequired后调用

		GridMap& getLowLevelMap() const;
		const VectorI& getMaxClusterSize() const;
		const Array2D<HpaCluster*>& getClusters() const;
		const AbstractGraph& getAbstractGraph() const;

		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	protected:		
		void createClusterAndBridges();
		void buildAbstractGraph();
		void buildAbstractGraph_addIntraEdgesInCluster(HpaCluster& cluster);
		void createVerticalBridges(int xStart, int xEnd, int y, HpaCluster& cluster2);
		void createHorizontalBridges(int yStart, int yEnd, int x, HpaCluster& cluster2);
		std::pair<AbstractNode*, bool> addStartOrTargetNodeForHpa(int lowLevelNodeID, bool bStart);
		HpaCluster& getClusterOfLowLevelNode(int lowLevelNodeID) const;
		void abstractToLowLevelPath(const std::vector<int>& abstract, std::vector<int>& lowLevel) const;
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		Array2D<HpaCluster*> m_clusters;		
		AbstractGraph m_abstractGraph;
		std::vector<Bridge> m_bridges;
	};

	class HpaCluster
		: public GridMapPart
	{
		friend class HpaMap;
		friend class Hpa;
	public:
		typedef VectorI ClusterCoord;
		typedef std::vector<HpaMap::AbstractNode*> Ports;
		const ClusterCoord& getClusterCoord() const;
		const Ports& getPorts() const;			
	private:
		HpaMap::AbstractNode* findPortWithLowLevelNodeID(int lowLevelNodeID) const;
		void localToLowLevelPath(const std::vector<int>& local, std::vector<int>& lowLevel) const;
		HpaCluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord);
		ClusterCoord m_clusterCoord;
		Ports m_ports;
	};

	class Hpa
		: public PathFinder
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
		ErrorType search();
		int popNextPathNode();
		ErrorType getError() const;
		const std::vector<int>& getRoughPath() const;
	protected:		
		void _search();
		HpaMap& m_env;		
		int m_lowLevelStartNodeID;
		int m_lowLevelTargetNodeID;
		int m_minClearanceValueRequired;
		bool m_bInited;
		ErrorType m_error;
		std::vector<int> m_roughPath;
		std::vector<int> m_localRefinedPath;
	};

	inline HpaCluster::HpaCluster(GridMap& orignMap, const Range& range, const ClusterCoord& clusterCoord)
		: GridMapPart(orignMap, range)
		, m_clusterCoord(clusterCoord)
	{
	}

	inline const HpaCluster::ClusterCoord& HpaCluster::getClusterCoord() const
	{
		return m_clusterCoord;
	}

	inline const HpaCluster::Ports& HpaCluster::getPorts() const
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

	inline const Array2D<HpaCluster*>& HpaMap::getClusters() const
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
