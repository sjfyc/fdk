#include <fdkgame/FindPathHpa.h>
#include <fdkgame/FindPathAStar.h>

namespace fdk { namespace game { namespace findpath
{
	AbstractGridMap::AbstractGridMap(GridMap& lowLevelMap, const VectorI& maxClusterSize)
		: m_lowLevelMap(lowLevelMap)
		, m_maxClusterSize(maxClusterSize)
	{
		FDK_ASSERT(maxClusterSize.x > 1 && maxClusterSize.y > 1);
	}

	void AbstractGridMap::clear()
	{
		for (size_t i = 0; i < m_clusters.count(); ++i)
		{
			Cluster* cluster = m_clusters.raw_data()[i];
			delete cluster;
		}
		m_entrances.clear();
		m_abstractGraph.clear();		
	}

	void AbstractGridMap::rebuildAbstract()
	{
		clear();
		createClusterAndEntrances();
		buildAbstractGraph();
	}

	void AbstractGridMap::createClusterAndEntrances()
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
					createHorizontalEntrances(x, x+clusterSize.x-1, y-1, *cluster);
				}
				if(x > 0)
				{
					createVerticalEntrances(y, y+clusterSize.y-1, x-1, *cluster);
				}
				++clusterCoord.x;
			}
			++clusterCoord.y;
		}
	}

	void AbstractGridMap::buildAbstractGraph()
	{
		for (size_t i = 0; i < m_entrances.size(); ++i)
		{
			Entrance& entrance = m_entrances[i];

			AbstractNodeInfo absNodeInfo;

			absNodeInfo.lowLevelNodeID = entrance.lowLevelNode1ID;
			AbstractNode* absNode1 = &m_abstractGraph.addNode(2*i, absNodeInfo);
			entrance.cluster1->m_entrances.push_back(absNode1);

			absNodeInfo.lowLevelNodeID = entrance.lowLevelNode2ID;
			AbstractNode* absNode2 = &m_abstractGraph.addNode(2*i+1, absNodeInfo);
			entrance.cluster2->m_entrances.push_back(absNode2);

			AbstractEdgeInfo absEdgeInfo;
			absEdgeInfo.cost = GridMap::COST_STRAIGHT;
			m_abstractGraph.addEdge(*absNode1, *absNode2, absEdgeInfo);
			m_abstractGraph.addEdge(*absNode2, *absNode1, absEdgeInfo);
		}
		
		for (size_t iCluster = 0; iCluster < m_clusters.count(); ++iCluster)
		{
			Cluster& cluster = *m_clusters.raw_data()[iCluster];

			Array2D<bool> hasComputed(cluster.getRange().area(), cluster.getRange().area(), false);
			Array2D<int> computedCost(cluster.getRange().area(), cluster.getRange().area(), PATHUNEXIST_COST);
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
					int localNode1ID = cluster.toPartNodeID(entrance1->getInfo().lowLevelNodeID);
					int localNode2ID = cluster.toPartNodeID(entrance2->getInfo().lowLevelNodeID);
					
					if (hasComputed(localNode1ID, localNode2ID))
					{
						int cost = computedCost(localNode1ID, localNode2ID);
						if (cost != PATHUNEXIST_COST)
						{
							AbstractEdgeInfo absEdgeInfo;
							absEdgeInfo.cost = cost;
							m_abstractGraph.addEdge(*entrance1, *entrance2, absEdgeInfo);
						}
						continue;
					}

					hasComputed(localNode1ID, localNode2ID) = true;
					hasComputed(localNode2ID, localNode1ID) = true;
					AStar astar(cluster, 
						cluster.toPartNodeID(entrance1->getInfo().lowLevelNodeID), 
						cluster.toPartNodeID(entrance2->getInfo().lowLevelNodeID));
					if (astar.search() == AStar::SearchResult_Completed)
					{
						AbstractEdgeInfo absEdgeInfo;
						absEdgeInfo.cost = astar.getPathCost();
						m_abstractGraph.addEdge(*entrance1, *entrance2, absEdgeInfo);
						computedCost(localNode1ID, localNode2ID) = absEdgeInfo.cost;
					}
				}
			}
		}
	}

	void AbstractGridMap::createHorizontalEntrances(int xStart, int xEnd, int y, Cluster& cluster2)
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

			int entranceStart = x;
			int entranceEnd;
			while (1)
			{
				entranceEnd = x;

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
			Entrance entrance;
			entrance.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(entranceStart, y)+VectorI(entranceEnd, y))/2 );
			entrance.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(entranceStart, y+1)+VectorI(entranceEnd, y+1))/2 );
			entrance.cluster1 = m_clusters(cluster2.getClusterCoord().x, cluster2.getClusterCoord().y-1);
			entrance.cluster2 = &cluster2;
			m_entrances.push_back(entrance);
		}
	}

	void AbstractGridMap::createVerticalEntrances(int yStart, int yEnd, int x, Cluster& cluster2)
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

			int entranceStart = y;
			int entranceEnd;
			while (1)
			{
				entranceEnd = y;

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
			Entrance entrance;
			entrance.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(x, entranceStart)+VectorI(x, entranceEnd))/2 );
			entrance.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(x+1, entranceStart)+VectorI(x+1, entranceEnd))/2 );
			entrance.cluster1 = m_clusters(cluster2.getClusterCoord().x-1, cluster2.getClusterCoord().y);
			entrance.cluster2 = &cluster2;
			m_entrances.push_back(entrance);
		}
	}

	int AbstractGridMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		const AbstractNode& startAbsNode = m_abstractGraph.getNode(startNodeID);
		const AbstractNode& targetAbsNode = m_abstractGraph.getNode(targetNodeID);
		return m_lowLevelMap.getHeuristic(startAbsNode.getInfo().lowLevelNodeID, targetAbsNode.getInfo().lowLevelNodeID);
	}

	void AbstractGridMap::getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const
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

	bool AbstractGridMap::isObstacle(int nodeID) const
	{
		return false;
	}
	
	std::pair<AbstractGridMap::AbstractNode*, bool> AbstractGridMap::addStartOrTargetNodeAfterBuildedAbstract(int lowLevelNodeID, bool bStart)
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

	AbstractGridMap::Cluster& AbstractGridMap::getClusterOfLowLevelNode(int lowLevelNodeID) const
	{
		GridMap::NodeCoord lowLevelNodeCoord = m_lowLevelMap.getNodeCoord(lowLevelNodeID);
		ClusterCoord::ValueType clusterCoordX = lowLevelNodeCoord.x/m_maxClusterSize.x;
		ClusterCoord::ValueType clusterCoordY = lowLevelNodeCoord.y/m_maxClusterSize.y;
		return *m_clusters(clusterCoordX, clusterCoordY);
	}

	AbstractGridMap::AbstractNode* AbstractGridMap::Cluster::findEntranceWithLowLevelNodeID(int lowLevelNodeID) const
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

	Hpa::Hpa(AbstractGridMap& env, int startNodeID, int targetNodeID)
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
			AbstractGridMap::AbstractNode* absNode = m_tempAddedStartTarget[i];
			m_env.m_abstractGraph.removeNode(*absNode);
		}
	}

	Hpa::SearchResult Hpa::search(int step)
	{
		//if (m_searchResult != SearchResult_Proceeding)
		//{
		//	return m_searchResult;
		//}
		//int proceededStep = 0;
		//std::vector<SuccessorNodeInfo> successors;
		//while (!m_openList.empty())
		//{
		//	OpenListItem current = m_openList.top();			
		//	m_openList.pop();
		//	m_nodeStates[current.nodeID] = NodeState_Closed;
		//	if (m_recorder)
		//	{
		//		m_recorder->onCloseNode(current.nodeID);
		//	}

		//	if (current.nodeID == m_targetNodeID)
		//	{
		//		buildPath();
		//		m_pathCost = current.fValue;
		//		m_searchResult = SearchResult_Completed;
		//		return SearchResult_Completed;
		//	}			

		//	successors.clear();
		//	m_env.getSuccessorNodes(current.nodeID, successors);
		//	for (size_t i = 0; i < successors.size(); ++i)
		//	{
		//		SuccessorNodeInfo& successor = successors[i];
		//		inspectNode(successor.nodeID, current.nodeID,
		//			m_nodeDatas[current.nodeID].gValue+successor.cost);
		//	}

		//	++proceededStep;
		//	if (step >=1 && proceededStep >= step)
		//	{
		//		return SearchResult_Proceeding;
		//	}
		//}
		//m_searchResult = SearchResult_PathUnexist;
		return SearchResult_PathUnexist;
	}

	void Hpa::initSearch()
	{
		// 处于相同的cluster, 直接在局部搜索
		AbstractGridMap::Cluster& startCluster = m_env.getClusterOfLowLevelNode(m_startNodeID);
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
		std::pair<AbstractGridMap::AbstractNode*, bool> resultPair;
		AbstractGridMap::AbstractNode* startAbsNode;
		AbstractGridMap::AbstractNode* targetAbsNode;
		
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

				/*		AbstractGridMap::Cluster& startCluster = getClusterOfLowLevelNode(startAbsNode.getInfo().lowLevelNodeID);
				AbstractGridMap::Cluster& targetCluster = getClusterOfLowLevelNode(targetAbsNode.getInfo().lowLevelNodeID);
				if (&startCluster != &targetCluster)
				{
				output.push_back(targetAbsNode.getInfo().lowLevelNodeID);			
				}
				else
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
				}*/


			}			
		}
		return INVALID_NODEID;
	}

}}}


