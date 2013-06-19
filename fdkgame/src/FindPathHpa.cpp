#include <fdkgame/FindPathHpa.h>
#include <map>
#include <fdkgame/FindPathAStar.h>

namespace fdk { namespace game { namespace findpath
{
	HpaMap::HpaMap(GridMap& lowLevelMap, const VectorI& maxClusterSize)
		: m_lowLevelMap(lowLevelMap)
		, m_maxClusterSize(maxClusterSize)
	{
		FDK_ASSERT(maxClusterSize.x > 1 && maxClusterSize.y > 1);
	}

	void HpaMap::clear()
	{
		for (size_t i = 0; i < m_clusters.count(); ++i)
		{
			Cluster* cluster = m_clusters.raw_data()[i];
			delete cluster;
		}
		m_clusters.clear();		
		m_abstractGraph.clear();
		m_bridges.clear();
	}

	void HpaMap::rebuildAbstract()
	{
		clear();
		createClusterAndBridges();
		buildAbstractGraph();
	}

	void HpaMap::createClusterAndBridges()
	{
		const GridMap::MapData& lowLevelMapData = m_lowLevelMap.getMapData();
		m_clusters.reset(
			lowLevelMapData.size_x()/m_maxClusterSize.x+(lowLevelMapData.size_x()%m_maxClusterSize.x > 0 ? 1 : 0),
			lowLevelMapData.size_y()/m_maxClusterSize.y+(lowLevelMapData.size_y()%m_maxClusterSize.y > 0 ? 1 : 0)
			);

		VectorI clusterSize;
		VectorI clusterCoord;
		clusterCoord.y = 0;
		for (int y = 0; y < (int)lowLevelMapData.size_y(); y += m_maxClusterSize.y)
		{
			clusterCoord.x = 0;
			for (int x = 0; x < (int)lowLevelMapData.size_x(); x += m_maxClusterSize.x)
			{
				clusterSize.x = minOf(m_maxClusterSize.x, (int)lowLevelMapData.size_x()-x);
				clusterSize.y = minOf(m_maxClusterSize.y, (int)lowLevelMapData.size_y()-y);

				FDK_ASSERT(!m_clusters(clusterCoord.x, clusterCoord.y));
				Cluster* cluster = new Cluster(m_lowLevelMap, Cluster::Range(VectorI(x, y), VectorI(x, y)+clusterSize), clusterCoord);
				m_clusters(clusterCoord.x, clusterCoord.y) = cluster;

				if (y > 0)
				{
					createVerticalBridges(x, x+clusterSize.x-1, y-1, *cluster);
				}
				if(x > 0)
				{
					createHorizontalBridges(y, y+clusterSize.y-1, x-1, *cluster);
				}
				++clusterCoord.x;
			}
			++clusterCoord.y;
		}
	}

	void HpaMap::buildAbstractGraph()
	{
		std::map<int, int> lowLevelID_to_absID;
		
		int absNodeID = 0;
		for (size_t i = 0; i < m_bridges.size(); ++i)
		{
			Bridge& entrance = m_bridges[i];			
			AbstractNode* absNode1;
			AbstractNode* absNode2;			

			{
				std::map<int, int>::iterator it = lowLevelID_to_absID.find(entrance.lowLevelNode1ID);
				if (it == lowLevelID_to_absID.end())
				{
					lowLevelID_to_absID.insert(std::make_pair(entrance.lowLevelNode1ID, absNodeID) );

					AbstractNodeInfo absNodeInfo;
					absNodeInfo.lowLevelNodeID = entrance.lowLevelNode1ID;

					absNode1 = &m_abstractGraph.addNode(absNodeID, absNodeInfo);
					entrance.cluster1->m_entrances.push_back(absNode1);

					++absNodeID;
				}
				else
				{
					absNode1 = &m_abstractGraph.getNode(it->second);
				}
			}			
			{
				std::map<int, int>::iterator it = lowLevelID_to_absID.find(entrance.lowLevelNode2ID);
				if (it == lowLevelID_to_absID.end())
				{
					lowLevelID_to_absID.insert(std::make_pair(entrance.lowLevelNode2ID, absNodeID) );

					AbstractNodeInfo absNodeInfo;
					absNodeInfo.lowLevelNodeID = entrance.lowLevelNode2ID;

					absNode2 = &m_abstractGraph.addNode(absNodeID, absNodeInfo);
					entrance.cluster2->m_entrances.push_back(absNode2);

					++absNodeID;
				}
				else
				{
					absNode2 = &m_abstractGraph.getNode(it->second);
				}
			}

			AbstractEdgeInfo absEdgeInfo;
			absEdgeInfo.cost = GridMap::COST_STRAIGHT;
			absEdgeInfo.bIntra = false;
			m_abstractGraph.addEdge(*absNode1, *absNode2, absEdgeInfo);
			m_abstractGraph.addEdge(*absNode2, *absNode1, absEdgeInfo);
		}

		for (size_t i = 0; i < m_clusters.count(); ++i)
		{
			Cluster& cluster = *m_clusters.raw_data()[i];
			buildAbstractGraph_addIntraEdgesInCluster(cluster);
		}
	}

	void HpaMap::buildAbstractGraph_addIntraEdgesInCluster(Cluster& cluster)
	{
		std::map<std::pair<AbstractNode*, AbstractNode*>, int> computedCostMap;

		for (size_t iEntrance1 = 0; iEntrance1 < cluster.m_entrances.size(); ++iEntrance1)
		{
			AbstractNode* entrance1 = cluster.m_entrances[iEntrance1];
			for (size_t iEntrance2 = 0; iEntrance2 < cluster.m_entrances.size(); ++iEntrance2)
			{
				AbstractNode* entrance2 = cluster.m_entrances[iEntrance2];
				if (entrance1 == entrance2)
				{
					continue;
				}
				std::map<std::pair<AbstractNode*, AbstractNode*>, int>::iterator it = 
					computedCostMap.find(std::make_pair(entrance1, entrance2) );
				if (it != computedCostMap.end())
				{
					int cost = it->second;
					if (cost != PATHUNEXIST_COST)
					{
						AbstractEdgeInfo absEdgeInfo;
						absEdgeInfo.bIntra = true;
						absEdgeInfo.cost = cost;
						m_abstractGraph.addEdge(*entrance1, *entrance2, absEdgeInfo);
					}
					continue;
				}

				int cost = PATHUNEXIST_COST;
				AStar astar(cluster, 
					cluster.toPartNodeID(entrance1->getInfo().lowLevelNodeID), 
					cluster.toPartNodeID(entrance2->getInfo().lowLevelNodeID));
				if (astar.search() == AStar::SearchResult_Completed)
				{
					AbstractEdgeInfo absEdgeInfo;
					absEdgeInfo.bIntra = true;
					absEdgeInfo.cost = astar.getPathCost();
					m_abstractGraph.addEdge(*entrance1, *entrance2, absEdgeInfo);
					cost = absEdgeInfo.cost;
				}

				computedCostMap.insert(std::make_pair( 
					std::make_pair(entrance2, entrance1),
					cost
					));
			}
		}
	}

	void HpaMap::createVerticalBridges(int xStart, int xEnd, int y, Cluster& cluster2)
	{
		int node1ID, node2ID;

		for (int x = xStart; x <= xEnd; ++x)
		{			
			node1ID = m_lowLevelMap.getNodeID(VectorI(x, y));
			node2ID = m_lowLevelMap.getNodeID(VectorI(x, y+1));

			if (m_lowLevelMap.isObstacle(node1ID) || m_lowLevelMap.isObstacle(node2ID))
			{
				continue;
			}

			int start = x;
			int end;
			while (1)
			{
				end = x;

				++x;
				if (x > xEnd)
				{
					break;
				}

				node1ID = m_lowLevelMap.getNodeID(VectorI(x, y));
				node2ID = m_lowLevelMap.getNodeID(VectorI(x, y+1));
				if (m_lowLevelMap.isObstacle(node1ID) || m_lowLevelMap.isObstacle(node2ID))
				{
					break;
				}
			}

			// 在中间创建入口
			Bridge bridge;
			bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(start, y)+VectorI(end, y))/2 );
			bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(start, y+1)+VectorI(end, y+1))/2 );
			bridge.cluster1 = m_clusters(cluster2.getClusterCoord().x, cluster2.getClusterCoord().y-1);
			bridge.cluster2 = &cluster2;
			m_bridges.push_back(bridge);
		}
	}

	void HpaMap::createHorizontalBridges(int yStart, int yEnd, int x, Cluster& cluster2)
	{
		int node1ID, node2ID;

		for (int y = yStart; y <= yEnd; ++y)
		{			
			node1ID = m_lowLevelMap.getNodeID(VectorI(x, y));
			node2ID = m_lowLevelMap.getNodeID(VectorI(x+1, y));

			if (m_lowLevelMap.isObstacle(node1ID) || m_lowLevelMap.isObstacle(node2ID))
			{
				continue;
			}

			int start = y;
			int end;
			while (1)
			{
				end = y;

				++y;
				if (y > yEnd)
				{
					break;
				}

				node1ID = m_lowLevelMap.getNodeID(VectorI(x, y));
				node2ID = m_lowLevelMap.getNodeID(VectorI(x+1, y));
				if (m_lowLevelMap.isObstacle(node1ID) || m_lowLevelMap.isObstacle(node2ID))
				{
					break;
				}
			}

			// 在中间创建入口
			Bridge bridge;
			bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(x, start)+VectorI(x, end))/2 );
			bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(x+1, start)+VectorI(x+1, end))/2 );
			bridge.cluster1 = m_clusters(cluster2.getClusterCoord().x-1, cluster2.getClusterCoord().y);
			bridge.cluster2 = &cluster2;
			m_bridges.push_back(bridge);
		}
	}

	int HpaMap::getNodeSpaceSize() const
	{
		return m_abstractGraph.getNodes().size();
	}

	int HpaMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		const AbstractNode& startAbsNode = m_abstractGraph.getNode(startNodeID);
		const AbstractNode& targetAbsNode = m_abstractGraph.getNode(targetNodeID);
		return m_lowLevelMap.getHeuristic(startAbsNode.getInfo().lowLevelNodeID, targetAbsNode.getInfo().lowLevelNodeID);
	}

	void HpaMap::getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		const AbstractNode::OutEdges& outAbsEdges = m_abstractGraph.getNode(nodeID).getOutEdges();
		for (AbstractNode::OutEdges::const_iterator it = outAbsEdges.begin(); 
			it != outAbsEdges.end(); ++it)
		{
			const AbstractEdge* absEdge = *it;
			SuccessorNodeInfo info;
			info.nodeID = absEdge->getTargetNode().getID();
			info.cost = absEdge->getInfo().cost;
		}
	}

	bool HpaMap::isObstacle(int nodeID) const
	{
		return false;
	}
	
	std::pair<HpaMap::AbstractNode*, bool> HpaMap::addStartOrTargetNodeAfterBuildedAbstract(int lowLevelNodeID, bool bStart)
	{
		Cluster& cluster = getClusterOfLowLevelNode(lowLevelNodeID);
		AbstractNode* abstractNode = cluster.findEntranceWithLowLevelNodeID(lowLevelNodeID);
		if (abstractNode)
		{
			return std::make_pair(abstractNode, false);
		}
		AbstractNodeInfo absNodeInfo;
		absNodeInfo.lowLevelNodeID = lowLevelNodeID;
		abstractNode = &m_abstractGraph.addNode(m_abstractGraph.getNodes().size(), absNodeInfo);

		AbstractNode* startAbsNode = bStart ? abstractNode : 0;
		AbstractNode* targetAbsNode = bStart ? 0 : abstractNode;
		for (size_t i = 0; i < cluster.m_entrances.size(); ++i)
		{
			AbstractNode* entrance = cluster.m_entrances[i];
			if (bStart)
			{
				targetAbsNode = entrance;
			}
			else
			{
				startAbsNode = entrance;
			}
			if (bStart)
			{
				AStar astar(cluster, 
					cluster.toPartNodeID(startAbsNode->getInfo().lowLevelNodeID), 
					cluster.toPartNodeID(targetAbsNode->getInfo().lowLevelNodeID));
				if (astar.search() == AStar::SearchResult_Completed)
				{
					AbstractEdgeInfo absEdgeInfo;
					absEdgeInfo.cost = astar.getPathCost();
					m_abstractGraph.addEdge(*startAbsNode, *targetAbsNode, absEdgeInfo);
				}
			}
		}
		return std::make_pair(abstractNode, true);
	}

	HpaMap::Cluster& HpaMap::getClusterOfLowLevelNode(int lowLevelNodeID) const
	{
		GridMap::NodeCoord lowLevelNodeCoord = m_lowLevelMap.getNodeCoord(lowLevelNodeID);
		ClusterCoord::ValueType clusterCoordX = lowLevelNodeCoord.x/m_maxClusterSize.x;
		ClusterCoord::ValueType clusterCoordY = lowLevelNodeCoord.y/m_maxClusterSize.y;
		return *m_clusters(clusterCoordX, clusterCoordY);
	}

	HpaMap::AbstractNode* HpaMap::Cluster::findEntranceWithLowLevelNodeID(int lowLevelNodeID) const
	{
		for (size_t i = 0; i < m_entrances.size(); ++i)
		{
			AbstractNode* abstractNode = m_entrances[i];
			if (abstractNode->getInfo().lowLevelNodeID == lowLevelNodeID)
			{
				return abstractNode;
			}
		}
		return 0;
	}

	Hpa::Hpa(HpaMap& env, int startNodeID, int targetNodeID)
		: m_env(env)
		, m_startNodeID(startNodeID)
		, m_targetNodeID(targetNodeID)
		, m_searchResult(SearchResult_Proceeding)
		, m_path()
		, m_pathCost(PATHUNEXIST_COST)
	{
		FDK_ASSERT(startNodeID != targetNodeID);
		FDK_ASSERT(m_env.getLowLevelMap().isValidNodeID(startNodeID));
		FDK_ASSERT(m_env.getLowLevelMap().isValidNodeID(targetNodeID));
		initSearch();
	}

	Hpa::~Hpa()
	{
		for (size_t i = 0; i < m_tempAddedStartTarget.size(); ++i)
		{
			HpaMap::AbstractNode* absNode = m_tempAddedStartTarget[i];
			m_env.m_abstractGraph.removeNode(*absNode);
		}
	}

	void Hpa::initSearch()
	{
		// 处于相同的cluster, 直接在局部搜索
		HpaMap::Cluster& startCluster = m_env.getClusterOfLowLevelNode(m_startNodeID);
		if (&startCluster == &m_env.getClusterOfLowLevelNode(m_targetNodeID))
		{
			AStar astar(startCluster, 
				startCluster.toPartNodeID(m_startNodeID), 
				startCluster.toPartNodeID(m_targetNodeID));
			if (astar.search() == AStar::SearchResult_Completed)
			{
				m_searchResult = SearchResult_Completed;
				m_path = astar.getPath();
				m_pathCost = astar.getPathCost();
				return;
			}
		}

		// 否则先搜一条抽象路径
		std::pair<HpaMap::AbstractNode*, bool> resultPair;
		HpaMap::AbstractNode* startAbsNode;
		HpaMap::AbstractNode* targetAbsNode;
		
		resultPair = m_env.addStartOrTargetNodeAfterBuildedAbstract(m_startNodeID, true);
		startAbsNode = resultPair.first;
		if (resultPair.second)
		{
			m_tempAddedStartTarget.push_back(startAbsNode);
		}
		resultPair = m_env.addStartOrTargetNodeAfterBuildedAbstract(m_targetNodeID, false);
		targetAbsNode = resultPair.first;
		if (resultPair.second)
		{
			m_tempAddedStartTarget.push_back(targetAbsNode);
		}

		// 如果抽象路径搜索失败则不存在路径
		AStar astar(m_env, startAbsNode->getID(), targetAbsNode->getID());
		if (astar.search() == AStar::SearchResult_PathUnexist)
		{
			m_searchResult = SearchResult_PathUnexist;
			return;
		}

		m_path = astar.getPath();
		m_path.push_back(startAbsNode->getID());
		m_localRefinedPath.clear();
	}

	int Hpa::popNextPathNode()
	{
		if (m_searchResult == SearchResult_PathUnexist)
		{
			return INVALID_NODEID;
		}
		
		if (m_searchResult == SearchResult_Completed)
		{
			if (!m_path.empty())
			{
				int nodeID = m_path.back();
				m_path.pop_back();
				return nodeID;
			}
			else
			{
				return INVALID_NODEID;
			}
		}

		if (m_localRefinedPath.empty())
		{
			if (m_path.size() >= 2)
			{
				int startAbsNodeID = m_path.back();
				m_path.pop_back();
				int targetAbsNodeID = m_path.back();				

				HpaMap::AbstractNode& startAbsNode = m_env.m_abstractGraph.getNode(startAbsNodeID);
				HpaMap::AbstractNode& targetAbsNode = m_env.m_abstractGraph.getNode(targetAbsNodeID);

				int startNodeID = startAbsNode.getInfo().lowLevelNodeID;
				int targetNodeID = targetAbsNode.getInfo().lowLevelNodeID;

				HpaMap::Cluster& startCluster = m_env.getClusterOfLowLevelNode(startNodeID);
				HpaMap::Cluster& targetCluster = m_env.getClusterOfLowLevelNode(targetNodeID);
				if (&startCluster != &targetCluster)
				{
					m_localRefinedPath.push_back(targetAbsNode.getInfo().lowLevelNodeID);			
				}
				else
				{
					AStar astar(startCluster, 
						startCluster.toPartNodeID(startNodeID), 
						startCluster.toPartNodeID(targetNodeID));
					if (astar.search() == AStar::SearchResult_Completed)
					{
						m_localRefinedPath = astar.getPath();
					}
					else
					{
						m_searchResult = SearchResult_PathUnexist;
						return INVALID_NODEID;
					}
				}
			}
			else
			{
				m_searchResult = SearchResult_Completed;
				m_path.clear();
				return INVALID_NODEID;
			}
		}
		
		int node = m_localRefinedPath.back();
		m_localRefinedPath.pop_back();
		return node;
	}

}}}


