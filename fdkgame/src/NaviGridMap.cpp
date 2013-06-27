#include <fdkgame/NaviGridMap.h>
#include <math.h>
#include <utility>

namespace fdk { namespace game { namespace navi
{
	void GridMap::clearObstacles()
	{
		for (size_t y = 0; y < m_nodes.size_y(); ++y)
		{
			for (size_t x = 0; x < m_nodes.size_x(); ++x)
			{
				m_nodes(x, y).clearanceValue = -1;
			}
		}
	}

	int GridMap::getSizeX() const
	{
		return (int)m_nodes.size_x();
	}

	int GridMap::getSizeY() const
	{
		return (int)m_nodes.size_y();
	}


	int GridMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		return getHeuristic(toNodeCoord(startNodeID), toNodeCoord(targetNodeID));
	}

	int GridMap::getHeuristic(const VertexCoord& startVertexCoord, const VertexCoord& targetVertexCoord) const
	{		
		//{// Manhattan
		//	VertexCoord startToTarget = targetVertexCoord - startVertexCoord;
		//	return COST_STRAIGHT * (abs(startToTarget.x) + abs(startToTarget.y));
		//}
		//{// Chebyshev
		//	return COST_STRAIGHT * maxOf(abs(startVertexCoord.x - targetVertexCoord.x), abs(startVertexCoord.y - targetVertexCoord.y));
		//}
		{// Euclidean
			VertexCoord startToTarget = targetVertexCoord - startVertexCoord;
			return COST_STRAIGHT * startToTarget.length();
		}
	}

	void GridMap::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		NodeCoord coord = toNodeCoord(nodeID);

		const bool bLeft = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);

		if (bLeft && bTop)
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bTop && bRight)
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bRight && bBottom)
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (bBottom && bLeft)
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}		
	}

	bool GridMap::tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& coord, int cost, int parentNodeID) const
	{
		if (!isValidNodeCoord(coord))
		{
			return false;
		}
		const int nodeID = toNodeID(coord);
		if (!meetMinClearanceValueRequired(nodeID))
		{
			return false;
		}
		//if (navigator.getEnvironmentChecker() &&
		//	!navigator.getEnvironmentChecker()->checkSuccessorNode(*this, nodeID, parentNodeID))
		//{
		//	return false;
		//}
		SuccessorNodeInfo info;
		info.nodeID = nodeID;
		info.cost = cost;
		result.push_back(info);
		return true;
	}
	
	void GridMap::annotateMap()
	{
		for (int x = m_nodes.size_x()-1; x >= 0; --x)
		{
			for (int y = m_nodes.size_y()-1; y>=0; --y)
			{
				annotateNode(NodeCoord(x, y));
			}
		}
	}

	void GridMap::annotateNode(const NodeCoord& coord)
	{
		if (!isNodeReachable(toNodeID(coord)))
		{
			return;
		}

		const NodeCoord bottomNodeCoord(coord.x, coord.y+1);
		const NodeCoord rightNodeCoord(coord.x+1, coord.y);		
		const NodeCoord bottomRightNodeCoord(coord.x+1, coord.y+1);
		if (!isValidNodeCoord(rightNodeCoord) || 
			!isValidNodeCoord(bottomNodeCoord) || 			
			!isValidNodeCoord(bottomRightNodeCoord) )
		{
			m_nodes(coord.x, coord.y).clearanceValue = 1;
			return;
		}

		const Node& bottomNode = m_nodes(bottomNodeCoord.x, bottomNodeCoord.y);
		const Node& rightNode = m_nodes(rightNodeCoord.x, rightNodeCoord.y);
		const Node& bottomRightNode = m_nodes(bottomRightNodeCoord.x, bottomRightNodeCoord.y);
		if (bottomNode.clearanceValue == CLEARANCEVALUE_OBSTACLE ||
			rightNode.clearanceValue == CLEARANCEVALUE_OBSTACLE ||
			bottomRightNode.clearanceValue == CLEARANCEVALUE_OBSTACLE)
		{
			m_nodes(coord.x, coord.y).clearanceValue = 1;
			return;
		}

		int temp = minOf(			
			bottomNode.clearanceValue,
			rightNode.clearanceValue
			);
		m_nodes(coord.x, coord.y).clearanceValue = minOf(
			temp, 
			bottomRightNode.clearanceValue
			) + 1;
	}

	bool GridMap::isNodeReachable(int nodeID) const
	{
		if (!isValidNodeID(nodeID))
		{
			return false;
		}
		return m_nodes.raw_data()[nodeID].clearanceValue != CLEARANCEVALUE_OBSTACLE;
	}

}}}
