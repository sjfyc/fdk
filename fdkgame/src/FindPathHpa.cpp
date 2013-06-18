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
			Array2D<int> computedCost(cluster.getRange().area(), cluster.getRange().area(), AStar::NOPATH_COST);
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

	void AbstractGridMap::setStartTargetAfterBuildedAbstract(int lowLevelStartNodeID, int lowLevelTargetNodeID)
	{
		addStartOrTargetNodeAfterBuildedAbstract(lowLevelStartNodeID, true);
		addStartOrTargetNodeAfterBuildedAbstract(lowLevelTargetNodeID, false);
	}

	void AbstractGridMap::addStartOrTargetNodeAfterBuildedAbstract(int lowLevelNodeID, bool bStart)
	{
		Cluster& cluster = getClusterOfLowLevelNode(lowLevelNodeID);
		AbstractNode* abstractNode = cluster.findEntranceWithLowLevelNodeID(lowLevelNodeID);
		if (abstractNode)
		{
			return;
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

}}}

