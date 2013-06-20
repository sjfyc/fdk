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

	void HpaMap::clearAbstract()
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
		clearAbstract();
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
			Bridge& port = m_bridges[i];			
			AbstractNode* absNode1;
			AbstractNode* absNode2;			

			{
				std::map<int, int>::iterator it = lowLevelID_to_absID.find(port.lowLevelNode1ID);
				if (it == lowLevelID_to_absID.end())
				{
					lowLevelID_to_absID.insert(std::make_pair(port.lowLevelNode1ID, absNodeID) );

					AbstractNodeInfo absNodeInfo;
					absNodeInfo.lowLevelNodeID = port.lowLevelNode1ID;

					absNode1 = &m_abstractGraph.addNode(absNodeID, absNodeInfo);
					port.cluster1->m_ports.push_back(absNode1);

					++absNodeID;
				}
				else
				{
					absNode1 = &m_abstractGraph.getNode(it->second);
				}
			}			
			{
				std::map<int, int>::iterator it = lowLevelID_to_absID.find(port.lowLevelNode2ID);
				if (it == lowLevelID_to_absID.end())
				{
					lowLevelID_to_absID.insert(std::make_pair(port.lowLevelNode2ID, absNodeID) );

					AbstractNodeInfo absNodeInfo;
					absNodeInfo.lowLevelNodeID = port.lowLevelNode2ID;

					absNode2 = &m_abstractGraph.addNode(absNodeID, absNodeInfo);
					port.cluster2->m_ports.push_back(absNode2);

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

		for (size_t iPort1 = 0; iPort1 < cluster.m_ports.size(); ++iPort1)
		{
			AbstractNode* port1 = cluster.m_ports[iPort1];
			for (size_t iPort2 = 0; iPort2 < cluster.m_ports.size(); ++iPort2)
			{
				AbstractNode* port2 = cluster.m_ports[iPort2];
				if (port1 == port2)
				{
					continue;
				}
				std::map<std::pair<AbstractNode*, AbstractNode*>, int>::iterator it = 
					computedCostMap.find(std::make_pair(port1, port2) );
				if (it != computedCostMap.end())
				{
					int cost = it->second;
					if (cost != PATHUNEXIST_COST)
					{
						AbstractEdgeInfo absEdgeInfo;
						absEdgeInfo.bIntra = true;
						absEdgeInfo.cost = cost;
						m_abstractGraph.addEdge(*port1, *port2, absEdgeInfo);
					}
					continue;
				}

				int cost = PATHUNEXIST_COST;
				AStar astar(cluster, 
					cluster.toPartNodeID(port1->getInfo().lowLevelNodeID), 
					cluster.toPartNodeID(port2->getInfo().lowLevelNodeID));
				if (astar.search() == AStar::SearchResult_Completed)
				{
					AbstractEdgeInfo absEdgeInfo;
					absEdgeInfo.bIntra = true;
					absEdgeInfo.cost = astar.getPathCost();
					m_abstractGraph.addEdge(*port1, *port2, absEdgeInfo);
					cost = absEdgeInfo.cost;
				}

				computedCostMap.insert(std::make_pair( 
					std::make_pair(port2, port1),
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

			if (!m_lowLevelMap.meetMinClearanceValueRequired(node1ID) ||
				!m_lowLevelMap.meetMinClearanceValueRequired(node2ID) )
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
				if (!m_lowLevelMap.meetMinClearanceValueRequired(node1ID) ||
					!m_lowLevelMap.meetMinClearanceValueRequired(node2ID) )
				{
					break;
				}
			}

			Bridge bridge;
			bridge.cluster1 = m_clusters(cluster2.getClusterCoord().x, cluster2.getClusterCoord().y-1);
			bridge.cluster2 = &cluster2;

			if (end-start+1 > 6)
			{// 在两边创建port
				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID(VectorI(start, y));
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID(VectorI(start, y+1));
				m_bridges.push_back(bridge);

				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID(VectorI(end, y));
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID(VectorI(end, y+1));
				m_bridges.push_back(bridge);
			}
			else
			{// 在中间创建port
				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(start, y)+VectorI(end, y))/2 );
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(start, y+1)+VectorI(end, y+1))/2 );
				m_bridges.push_back(bridge);
			}
		}
	}

	void HpaMap::createHorizontalBridges(int yStart, int yEnd, int x, Cluster& cluster2)
	{
		int node1ID, node2ID;

		for (int y = yStart; y <= yEnd; ++y)
		{			
			node1ID = m_lowLevelMap.getNodeID(VectorI(x, y));
			node2ID = m_lowLevelMap.getNodeID(VectorI(x+1, y));

			if (!m_lowLevelMap.meetMinClearanceValueRequired(node1ID) ||
				!m_lowLevelMap.meetMinClearanceValueRequired(node2ID) )
			{
				continue;
			}

			int start = y;
			int end;

			int maxClearanceValue = maxOf(
				m_lowLevelMap.getClearanceValue(node1ID),
				m_lowLevelMap.getClearanceValue(node2ID)
				);
			int maxClearanceValueNode1ID = node1ID;
			int maxClearanceValueNode2ID = node2ID;

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
				if (!m_lowLevelMap.meetMinClearanceValueRequired(node1ID) ||
					!m_lowLevelMap.meetMinClearanceValueRequired(node2ID) )
				{
					break;
				}
			}
						
			Bridge bridge;
			bridge.cluster1 = m_clusters(cluster2.getClusterCoord().x-1, cluster2.getClusterCoord().y);
			bridge.cluster2 = &cluster2;

			int length = end-start+1;
			if (length > 6)
			{// 在两边创建port
				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID(VectorI(x, start));
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID(VectorI(x+1, start));
				m_bridges.push_back(bridge);

				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID(VectorI(x, end));
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID(VectorI(x+1, end));
				m_bridges.push_back(bridge);
				if (length > 12)
				{// 在中间创建port
					bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(x, start)+VectorI(x, end))/2 );
					bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(x+1, start)+VectorI(x+1, end))/2 );
					m_bridges.push_back(bridge);
				}
			}
			else
			{// 在中间创建port
				bridge.lowLevelNode1ID = m_lowLevelMap.getNodeID( (VectorI(x, start)+VectorI(x, end))/2 );
				bridge.lowLevelNode2ID = m_lowLevelMap.getNodeID( (VectorI(x+1, start)+VectorI(x+1, end))/2 );
				m_bridges.push_back(bridge);
			}
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

	void HpaMap::getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		const AbstractNode::OutEdges& outAbsEdges = m_abstractGraph.getNode(nodeID).getOutEdges();
		for (AbstractNode::OutEdges::const_iterator it = outAbsEdges.begin(); 
			it != outAbsEdges.end(); ++it)
		{
			const AbstractEdge* absEdge = *it;
			const AbstractNode& absTargetNode = absEdge->getTargetNode();
			if (!m_lowLevelMap.meetMinClearanceValueRequired(absTargetNode.getInfo().lowLevelNodeID))
			{
				continue;
			}
			SuccessorNodeInfo info;
			info.nodeID = absTargetNode.getID();
			info.cost = absEdge->getInfo().cost;
			result.push_back(info);
		}
	}

	bool HpaMap::isObstacle(int nodeID) const
	{
		return false;
	}
	
	std::pair<HpaMap::AbstractNode*, bool> HpaMap::addStartOrTargetNodeForHpa(int lowLevelNodeID, bool bStart)
	{
		Cluster& cluster = getClusterOfLowLevelNode(lowLevelNodeID);
		AbstractNode* abstractNode = cluster.findPortWithLowLevelNodeID(lowLevelNodeID);
		if (abstractNode)
		{
			return std::make_pair(abstractNode, false);
		}
		AbstractNodeInfo absNodeInfo;
		absNodeInfo.lowLevelNodeID = lowLevelNodeID;
		abstractNode = &m_abstractGraph.addNode(m_abstractGraph.getNodes().size(), absNodeInfo);

		AbstractNode* startAbsNode = bStart ? abstractNode : 0;
		AbstractNode* targetAbsNode = bStart ? 0 : abstractNode;
		for (size_t i = 0; i < cluster.m_ports.size(); ++i)
		{
			AbstractNode* port = cluster.m_ports[i];
			if (bStart)
			{
				targetAbsNode = port;
			}
			else
			{
				startAbsNode = port;
			}

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
		return std::make_pair(abstractNode, true);
	}

	HpaMap::Cluster& HpaMap::getClusterOfLowLevelNode(int lowLevelNodeID) const
	{
		GridMap::NodeCoord lowLevelNodeCoord = m_lowLevelMap.getNodeCoord(lowLevelNodeID);
		ClusterCoord::ValueType clusterCoordX = lowLevelNodeCoord.x/m_maxClusterSize.x;
		ClusterCoord::ValueType clusterCoordY = lowLevelNodeCoord.y/m_maxClusterSize.y;
		return *m_clusters(clusterCoordX, clusterCoordY);
	}

	void HpaMap::abstractToLowLevelPath(const std::vector<int>& abstract, std::vector<int>& lowLevel) const
	{
		for (size_t i = 0; i < abstract.size(); ++i)
		{
			const HpaMap::AbstractNode& absNode = m_abstractGraph.getNode(abstract[i]);
			lowLevel.push_back(absNode.getInfo().lowLevelNodeID);
		}
	}

	HpaMap::AbstractNode* HpaMap::Cluster::findPortWithLowLevelNodeID(int lowLevelNodeID) const
	{
		for (size_t i = 0; i < m_ports.size(); ++i)
		{
			AbstractNode* abstractNode = m_ports[i];
			if (abstractNode->getInfo().lowLevelNodeID == lowLevelNodeID)
			{
				return abstractNode;
			}
		}
		return 0;
	}

	void HpaMap::Cluster::localToLowLevelPath(const std::vector<int>& local, std::vector<int>& lowLevel) const
	{
		for (size_t i = 0; i < local.size(); ++i)
		{
			lowLevel.push_back(toOrignNodeID(local[i]) );
		}
	}

	Hpa::Hpa(HpaMap& env, int startNodeID, int targetNodeID, int minClearanceValueRequired)
		: m_env(env)
		, m_lowLevelStartNodeID(startNodeID)
		, m_lowLevelTargetNodeID(targetNodeID)
		, m_minClearanceValueRequired(minClearanceValueRequired)
		, m_error(Error_OK)
		, m_roughPath()
	{
		FDK_ASSERT(startNodeID != targetNodeID);
		FDK_ASSERT(m_env.getLowLevelMap().isValidNodeID(startNodeID));
		FDK_ASSERT(m_env.getLowLevelMap().isValidNodeID(targetNodeID));
		initSearch();
	}

	Hpa::~Hpa()
	{		
	}

	void Hpa::initSearch()
	{
		if (m_env.getLowLevelMap().isDirectlyReachable(m_lowLevelStartNodeID, m_lowLevelTargetNodeID))
		{
			m_localRefinedPath.push_back(m_lowLevelTargetNodeID);
			return;
		}

		// 处于相同的cluster, 直接在局部搜索
		HpaMap::Cluster& startCluster = m_env.getClusterOfLowLevelNode(m_lowLevelStartNodeID);
		if (&startCluster == &m_env.getClusterOfLowLevelNode(m_lowLevelTargetNodeID))
		{
			AStar astar(startCluster, 
				startCluster.toPartNodeID(m_lowLevelStartNodeID), 
				startCluster.toPartNodeID(m_lowLevelTargetNodeID),
				m_minClearanceValueRequired);
			if (astar.search() == AStar::SearchResult_Completed)
			{
				startCluster.localToLowLevelPath(astar.getPath(), m_localRefinedPath);
				return;
			}
		}

		// 否则先搜一条抽象路径
		std::pair<HpaMap::AbstractNode*, bool> resultPair;
		HpaMap::AbstractNode* startAbsNode;
		HpaMap::AbstractNode* targetAbsNode;
		std::vector<HpaMap::AbstractNode*> tempAddedStartTarget;

		resultPair = m_env.addStartOrTargetNodeForHpa(m_lowLevelStartNodeID, true);
		startAbsNode = resultPair.first;
		if (resultPair.second)
		{
			tempAddedStartTarget.push_back(startAbsNode);
		}
		resultPair = m_env.addStartOrTargetNodeForHpa(m_lowLevelTargetNodeID, false);
		targetAbsNode = resultPair.first;
		if (resultPair.second)
		{
			tempAddedStartTarget.push_back(targetAbsNode);
		}

		// 如果抽象路径搜索失败则不存在路径
		const int startAbsNodeID = startAbsNode->getID();
		const int targetAbsNodeID = targetAbsNode->getID();
		AStar astar(m_env, startAbsNodeID, targetAbsNodeID, m_minClearanceValueRequired);
		AStar::SearchResult searchResult = astar.search();
		
		if (searchResult == AStar::SearchResult_PathUnexist)
		{
			m_error = Error_PathUnexist;			
		}
		else
		{
			m_env.abstractToLowLevelPath(astar.getPath(), m_roughPath);
			m_roughPath.push_back(m_lowLevelStartNodeID);
			m_localRefinedPath.clear();
		}

		for (size_t i = 0; i < tempAddedStartTarget.size(); ++i)
		{
			HpaMap::AbstractNode* absNode = tempAddedStartTarget[i];
			m_env.m_abstractGraph.removeNode(*absNode);
		}
		return;
	}

	int Hpa::popNextPathNode()
	{
		if (m_error == Error_PathUnexist ||
			m_error == Error_PathCompleted)
		{
			return INVALID_NODEID;
		}

		if (m_localRefinedPath.empty())
		{
			if (m_roughPath.size() >= 2)
			{
				int startNodeID = m_roughPath.back();
				m_roughPath.pop_back();
				int targetNodeID = m_roughPath.back();

				HpaMap::Cluster& startCluster = m_env.getClusterOfLowLevelNode(startNodeID);
				HpaMap::Cluster& targetCluster = m_env.getClusterOfLowLevelNode(targetNodeID);
				if (&startCluster != &targetCluster) // must be inter-edge
				{
					m_localRefinedPath.push_back(targetNodeID);			
				}
				else
				{
					AStar astar(startCluster, 
						startCluster.toPartNodeID(startNodeID), 
						startCluster.toPartNodeID(targetNodeID),
						m_minClearanceValueRequired);
					if (astar.search() == AStar::SearchResult_Completed)
					{
						startCluster.localToLowLevelPath(astar.getPath(), m_localRefinedPath);
					}
					else
					{
						m_error = Error_PathUnexist;
						return INVALID_NODEID;
					}
				}
			}
			else
			{
				m_roughPath.clear();
				m_error = Error_PathCompleted;
				return INVALID_NODEID;
			}
		}
		
		int node = m_localRefinedPath.back();
		m_localRefinedPath.pop_back();
		return node;
	}

}}}
