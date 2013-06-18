#include <fdkgame/FindPathHpa.h>

namespace fdk { namespace game { namespace findpath
{
	AbstractGridMap::AbstractGridMap(GridMap& lowLevelMap, const VectorI& maxClusterSize)
		: m_lowLevelMap(lowLevelMap)
		, m_maxClusterSize(maxClusterSize)
	{
	}

	void AbstractGridMap::buildEntrances()
	{
		VectorI clusterSize;
		const GridMap::MapData& lowLevelMapData = m_lowLevelMap.getMapData();
		for (int y = 0; y < (int)lowLevelMapData.size_y(); y += m_maxClusterSize.y)
		{
			for (int x = 0; x < (int)lowLevelMapData.size_x(); x += m_maxClusterSize.x)
			{
				clusterSize.x = minOf(m_maxClusterSize.x, (int)lowLevelMapData.size_x()-x);
				clusterSize.y = minOf(m_maxClusterSize.y, (int)lowLevelMapData.size_y()-y);
				
				if (y > 0)
				{
					createHorizontalEntrances(x, x+clusterSize.x-1, y-1);
				}
				if(x > 0)
				{
					createVerticalEntrances(y, y+clusterSize.y-1, x-1);
				}
			}
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


}}}

