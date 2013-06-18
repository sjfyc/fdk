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

	void AbstractGridMap::buildAbstract()
	{		
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

			Array2D<bool> hasComputed(cluster.m_entrances.size(), cluster.m_entrances.size(), false);
			Array2D<int> computedCost(cluster.m_entrances.size(), cluster.m_entrances.size(), AStar::NOPATH_COST);
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
						if (cost != AStar::NOPATH_COST)
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

	void AbstractGridMap::createHorizontalEntrances(int xStart, int xEnd, int y, Cluster& cluster1)
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
			entrance.cluster1 = &cluster1;
			entrance.cluster2 = m_clusters(cluster1.getClusterCoord().x, cluster1.getClusterCoord().y+1);
			m_entrances.push_back(entrance);
		}
	}

	void AbstractGridMap::createVerticalEntrances(int yStart, int yEnd, int x, Cluster& cluster1)
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
			entrance.cluster1 = &cluster1;
			entrance.cluster2 = m_clusters(cluster1.getClusterCoord().x+1, cluster1.getClusterCoord().y);
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
		const AbstractNode& absNode = m_abstractGraph.getNode(nodeID);
		const AbstractNode::OutEdges& outAbsEdges = absNode.getOutEdges();
		
		//const vector<AbsTilingEdge>& edges = node.getOutEdges();
		//for (vector<AbsTilingEdge>::const_iterator i = edges.begin(); i != edges.end(); ++i)
		//{
		//	if (i->getInfo().getInter())
		//	{
		//		if (i->getInfo().getLevel() < m_currentLevel)
		//			continue;
		//	}
		//	else
		//	{
		//		if (i->getInfo().getLevel() != m_currentLevel)
		//			continue;
		//	}

		//	// 边的目标节点
		//	int targetNodeId = i->getTargetNodeId();
		//	assert(isValidNodeId(targetNodeId));
		//	const AbsTilingNodeInfo& targetNodeInfo = m_graph.getNodeInfo(targetNodeId);

		//	if (targetNodeInfo.getLevel() < m_currentLevel)
		//		continue;
		//	if (!nodeInCurrentCluster(targetNodeInfo))
		//		continue;
		//	if (lastNodeId != NO_NODE)
		//		if (pruneNode(targetNodeId, nodeId, lastNodeId))
		//			continue;
		//	result.push_back(Successor(targetNodeId, i->getInfo().getCost()));
		//}
	}

	bool AbstractGridMap::isObstacle(int nodeID) const
	{
		return false;
	}

	

}}}

