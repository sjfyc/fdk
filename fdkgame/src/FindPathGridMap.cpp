#include <fdkgame/FindPathGridMap.h>
#include <math.h>

namespace fdk { namespace game { namespace findpath
{
	const int COST_STRAIGHT = 100;	
	const int COST_DIAGONAL = 142;

	void GridMap::clearObstacles()
	{
		for (size_t y = 0; y < m_nodes.size_y(); ++y)
		{
			for (size_t x = 0; x < m_nodes.size_x(); ++x)
			{
				m_nodes(x, y).bObstacle = false;
			}
		}
	}

	int GridMap::getNodeCount() const
	{
		return m_nodes.count();
	}

	int GridMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		VectorI startToTarget = getNodeCoord(targetNodeID) - getNodeCoord(startNodeID);
		return (abs(startToTarget.x) + abs(startToTarget.y)) * COST_STRAIGHT;
	}

	void GridMap::getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		VectorI coord = getNodeCoord(nodeID);
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				if (x == 0 && y == 0)
				{
					continue;
				}
				VectorI aroundCoord(coord.x+x, coord.y+y);
				SuccessorNodeInfo info;
				info.nodeID = getNodeID(aroundCoord);
				if (info.nodeID == INVALID_NODEID)
				{
					continue;
				}				
				if (isObstacle(info.nodeID))
				{
					continue;
				}
				const bool bStraight = (x == 0 || y == 0);
				if (!bStraight)
				{// 横向纵向至少有一个可以移动才考虑斜向（两个都考虑就是别马脚算法）
					
				}
				info.cost = bStraight ? COST_STRAIGHT : COST_DIAGONAL;
				result.push_back(info);
			}
		}
	}

	bool GridMap::isObstacle(int nodeID) const
	{
		if (!isValidNodeID(nodeID))
		{
			return false;
		}
		return m_nodes.raw_data()[nodeID].bObstacle;
	}

	VectorI GridMap::getNodeCoord(int nodeID) const
	{
		return VectorI(nodeID%m_nodes.size_x(), nodeID/m_nodes.size_x());
	}

	int GridMap::getNodeID(const VectorI& coord) const
	{
		if (!isValidNodeCoord(coord))
		{
			return INVALID_NODEID;
		}
		return coord.y*m_nodes.size_x()+coord.x;
	}

	bool GridMap::isValidNodeCoord(const VectorI& coord) const
	{
		return coord.x >= 0 && coord.x < (int)m_nodes.size_x()
			&& coord.y >= 0 && coord.y < (int)m_nodes.size_y();
	}	
}}}
