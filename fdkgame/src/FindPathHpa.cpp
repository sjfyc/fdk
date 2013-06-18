#include <fdkgame/FindPathHpa.h>

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
				m_clusters(clusterCoord.x, clusterCoord.y) = new Cluster(m_lowLevelMap, Cluster::Range(VectorI(x, y), VectorI(x, y)+clusterSize), clusterCoord);

				if (y > 0)
				{
					createHorizontalEntrances(x, x+clusterSize.x-1, y-1);
				}
				if(x > 0)
				{
					createVerticalEntrances(y, y+clusterSize.y-1, x-1);
				}
				++clusterCoord.x;
			}
			++clusterCoord.y;
		}
		
		for (size_t i = 0; i < m_entrances.size(); ++i)
		{
			const Entrance& entrance = m_entrances[i];
			AbsNodeInfo absNodeInfo;
			absNodeInfo.lowLevelNodeID = entrance.node1ID;
			m_absGraph.addNode(2*i, absNodeInfo);
			absNodeInfo.lowLevelNodeID = entrance.node2ID;
			m_absGraph.addNode(2*i+1, absNodeInfo);
			AbsEdgeInfo absEdgeInfo;
			absEdgeInfo.cost = GridMap::COST_STRAIGHT;
			m_absGraph.addEdge(m_absGraph.getNode(2*i), m_absGraph.getNode(2*i+1), absEdgeInfo);
			m_absGraph.addEdge(m_absGraph.getNode(2*i+1), m_absGraph.getNode(2*i), absEdgeInfo);
		}
	}

	void AbstractGridMap::createHorizontalEntrances(int xStart, int xEnd, int y)
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
			entrance.node1ID = m_lowLevelMap.getNodeID( (VectorI(entranceStart, y)+VectorI(entranceEnd, y))/2 );
			entrance.node2ID = m_lowLevelMap.getNodeID( (VectorI(entranceStart, y+1)+VectorI(entranceEnd, y+1))/2 );
			m_entrances.push_back(entrance);
		}
	}

	void AbstractGridMap::createVerticalEntrances(int yStart, int yEnd, int x)
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
			entrance.node1ID = m_lowLevelMap.getNodeID( (VectorI(x, entranceStart)+VectorI(x, entranceEnd))/2 );
			entrance.node2ID = m_lowLevelMap.getNodeID( (VectorI(x+1, entranceStart)+VectorI(x+1, entranceEnd))/2 );
			m_entrances.push_back(entrance);
		}
	}

	int AbstractGridMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		const AbsNode& startAbsNode = m_absGraph.getNode(startNodeID);
		const AbsNode& targetAbsNode = m_absGraph.getNode(targetNodeID);
		return m_lowLevelMap.getHeuristic(startAbsNode.getInfo().lowLevelNodeID, targetAbsNode.getInfo().lowLevelNodeID);
	}

	void AbstractGridMap::getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		const AbsNode& absNode = m_absGraph.getNode(nodeID);
		const AbsNode::OutEdges& outAbsEdges = absNode.getOutEdges();
		
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

