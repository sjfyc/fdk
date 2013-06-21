#include <fdkgame/FindPathGridMap.h>
#include <math.h>
#include <utility>

namespace fdk { namespace game { namespace findpath
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

	int GridMap::getNodeSpaceSize() const
	{
		return m_nodes.count();
	}

	int GridMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		VectorI startToTarget = getNodeCoord(targetNodeID) - getNodeCoord(startNodeID);
		return (abs(startToTarget.x) + abs(startToTarget.y)) * COST_STRAIGHT;
	}

	void GridMap::getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		NodeCoord coord = getNodeCoord(nodeID);
	
		const bool bLeft = tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);
				
		if (bLeft && bTop)
		{
			tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bTop && bRight)
		{
			tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bRight && bBottom)
		{
			tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (bBottom && bLeft)
		{
			tryAddSuccessorNode(pathFinder, result, NodeCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}		
	}
	
	bool GridMap::tryAddSuccessorNode(PathFinder& pathFinder, std::vector<SuccessorNodeInfo>& result, const NodeCoord& coord, int cost, int parentNodeID) const
	{
		const int nodeID = getNodeID(coord);
		if (nodeID == INVALID_NODEID)
		{
			return false;
		}
		if (!meetMinClearanceValueRequired(nodeID))
		{
			return false;
		}
		if (pathFinder.getEnvironmentChecker() &&
			!pathFinder.getEnvironmentChecker()->checkSuccessorNode(*this, nodeID, parentNodeID))
		{
			return false;
		}
		SuccessorNodeInfo info;
		info.nodeID = nodeID;
		info.cost = cost;
		result.push_back(info);
		return true;
	}

	bool GridMap::isObstacle(int nodeID) const
	{
		if (!isValidNodeID(nodeID))
		{
			return false;
		}
		return m_nodes.raw_data()[nodeID].clearanceValue == CLEARANCEVALUE_OBSTACLE;
	}

	GridMap::NodeCoord GridMap::getNodeCoord(int nodeID) const
	{
		return NodeCoord(nodeID%m_nodes.size_x(), nodeID/m_nodes.size_x());
	}

	int GridMap::getNodeID(const NodeCoord& coord) const
	{
		if (!isValidNodeCoord(coord))
		{
			return INVALID_NODEID;
		}
		return coord.y * m_nodes.size_x() + coord.x;
	}

	bool GridMap::isValidNodeCoord(const NodeCoord& coord) const
	{
		return coord.x >= 0 && coord.x < (NodeCoord::ValueType)m_nodes.size_x()
			&& coord.y >= 0 && coord.y < (NodeCoord::ValueType)m_nodes.size_y();
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
		if (isObstacle(getNodeID(coord)))
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

	#if 0
	bool GridMap::isDirectlyReachable(int startNodeID, int targetNodeID) const
	{		
		NodeCoord startCoord = getNodeCoord(startNodeID);
		NodeCoord targetCoord = getNodeCoord(targetNodeID);

		int x1 = startCoord.x;
		int y1 = startCoord.y;
		int x2 = targetCoord.x;
		int y2 = targetCoord.y;
		
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);
		int yy = 0;

		if (dx < dy) 
		{
			yy = 1;
			std::swap(x1, y1);
			std::swap(x2, y2);
			std::swap(dx, dy);
		}

		int ix = (x2 - x1) > 0 ? 1 : -1;
		int iy = (y2 - y1) > 0 ? 1 : -1;
		int cx = x1;
		int cy = y1;
		int n2dy = dy * 2;
		int n2dydx = (dy - dx) * 2;
		int d = dy * 2 - dx;

		if (yy) 
		{ // 如果直线与 x 轴的夹角大于 45 度
			while (cx != x2) 
			{
				if (d < 0)
				{
					d += n2dy;
				}
				else 
				{
					cy += iy;
					d += n2dydx;
				}
				if (!meetMinClearanceValueRequired(getNodeID(NodeCoord(cy, cx)) ) )
				{
					return false;
				}
				cx += ix;
			}
		} 
		else 
		{ // 如果直线与 x 轴的夹角小于 45 度
			while (cx != x2) 
			{
				if (d < 0) 
				{
					d += n2dy;
				} 
				else 
				{
					cy += iy;
					d += n2dydx;
				}
				if (!meetMinClearanceValueRequired(getNodeID(NodeCoord(cx, cy)) ) )
				{
					return false;
				}
				cx += ix;
			}
		}
		return true;
	}
	#endif

	bool GridMap::isDirectlyReachable(int startNodeID, int targetNodeID, EnvironmentChecker* envChecker) const
	{
		if (envChecker)
		{
			envChecker->onSearchBegining(*this, startNodeID, targetNodeID);
		}
		bool result = _isDirectlyReachable(startNodeID, targetNodeID, envChecker);
		if (envChecker)
		{
			envChecker->onSearchEnded(*this, startNodeID, targetNodeID);
		}
		return result;
	}

	bool GridMap::_isDirectlyReachable(int startNodeID, int targetNodeID, EnvironmentChecker* envChecker) const
	{
		NodeCoord startCoord = getNodeCoord(startNodeID);
		NodeCoord targetCoord = getNodeCoord(targetNodeID);

		int startX = startCoord.x;
		int startY = startCoord.y;
		int targetX = targetCoord.x;
		int targetY = targetCoord.y;

		if (startX == targetX &&
			startY == targetY)
		{
			return true;
		}

		enum
		{
			DependOnX,
			DependOnY,
		} state;

		int x = startX;
		int y = startY;

		int dx;
		int sx;
		if( targetX - startX > 0)
		{
			dx = targetX - startX;
			sx = 1;
		}
		else if(targetX - startX < 0)
		{
			dx = startX - targetX;
			sx = -1;
		}
		else
		{
			dx = 0;
			sx = 0;
		}
		int dy;
		int sy;
		if( targetY - startY > 0)
		{
			dy = targetY - startY;
			sy = 1;
		}
		else if(targetY - startY < 0)
		{
			dy = startY - targetY;
			sy = -1;
		}
		else
		{
			dy = 0;
			sy = 0;
		}

		if(dy > dx)
		{
			std::swap(dy,dx);
			state = DependOnY;
		}
		else
		{
			state = DependOnX;
		}
		//initialize the error term to compensate for a nonezero intercept
		int NError = 2 * dy - dx;
		int nodeID;
		for (int i = 1; i <= dx; ++i)
		{
			nodeID = getNodeID(NodeCoord(x, y));
			if (!meetMinClearanceValueRequired(nodeID) )
			{
				return false;
			}
			if (envChecker && !envChecker->checkSuccessorNode(*this, nodeID, INVALID_NODEID))
			{
				return false;
			}
			if (NError >= 0)
			{
				if (state == DependOnY) 
				{
					nodeID = getNodeID(NodeCoord(x, y+sy));
					if (!meetMinClearanceValueRequired(nodeID ) )
					{
						return false;
					}
					if (envChecker && !envChecker->checkSuccessorNode(*this, nodeID, INVALID_NODEID))
					{
						return false;
					}
					x = x + sx;
					nodeID = getNodeID(NodeCoord(x, y));
					if (!meetMinClearanceValueRequired(nodeID ) )
					{
						return false;
					}
					if (envChecker && !envChecker->checkSuccessorNode(*this, nodeID, INVALID_NODEID))
					{
						return false;
					}
				}
				else
				{
					nodeID = getNodeID(NodeCoord(x+sx, y));
					if (!meetMinClearanceValueRequired(nodeID ) )
					{
						return false;
					}
					if (envChecker && !envChecker->checkSuccessorNode(*this, nodeID, INVALID_NODEID))
					{
						return false;
					}
					y = y + sy;
					nodeID = getNodeID(NodeCoord(x, y));
					if (!meetMinClearanceValueRequired(nodeID ) )
					{
						return false;
					}
					if (envChecker && !envChecker->checkSuccessorNode(*this, nodeID, INVALID_NODEID))
					{
						return false;
					}
				}
				NError = NError - 2 * dx;
			}
			if (state == DependOnY)
			{
				y = y + sy;
			}
			else
			{
				x = x + sx;
			}
			NError = NError + 2 * dy;
		}
		return true;
	}

	GridMapPart::GridMapPart(GridMap& orignMap, const Range& range)
		: m_orignMap(orignMap)
		, m_range(range)
	{
		m_range.normalize();
		m_range.clip(Range(0, 0, orignMap.getMapData().size_x(), orignMap.getMapData().size_y()));
		FDK_ASSERT(getNodeSpaceSize() > 0);
	}

	int GridMapPart::getNodeSpaceSize() const
	{
		return m_range.area();
	}

	int GridMapPart::getHeuristic(int startNodeID, int targetNodeID) const
	{
		return m_orignMap.getHeuristic(toOrignNodeID(startNodeID), toOrignNodeID(targetNodeID));
	}

	void GridMapPart::getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		int orignNodeID = toOrignNodeID(nodeID);
		std::vector<SuccessorNodeInfo> temp;
		m_orignMap.getSuccessorNodes(pathFinder, orignNodeID, temp);
		for (size_t i = 0; i < temp.size(); ++i)
		{
			SuccessorNodeInfo info = temp[i];
			int partNodeID = toPartNodeID(info.nodeID);
			if (partNodeID != INVALID_NODEID)
			{
				info.nodeID = partNodeID;
				result.push_back(info);
			}
		}
	}

	bool GridMapPart::isObstacle(int nodeID) const
	{
		return m_orignMap.isObstacle(toOrignNodeID(nodeID));
	}
	
	int GridMapPart::getPartNodeID(const PartNodeCoord& partNodeCoord) const
	{
		if (!isValidPartNodeCoord(partNodeCoord))
		{
			return INVALID_NODEID;
		}
		return partNodeCoord.y * m_range.width() + partNodeCoord.x;
	}

	bool GridMapPart::isValidPartNodeCoord(const PartNodeCoord& partNodeCoord) const
	{
		return partNodeCoord.x >= 0 && partNodeCoord.x < (PartNodeCoord::ValueType)m_range.width()
			&& partNodeCoord.y >= 0 && partNodeCoord.y < (PartNodeCoord::ValueType)m_range.height();
	}

	int GridMapPart::toOrignNodeID(int partNodeID) const
	{
		PartNodeCoord partNodeCoord = getPartNodeCoord(partNodeID);
		return m_orignMap.getNodeID(toOrignNodeCoord(partNodeCoord));
	}

	int GridMapPart::toPartNodeID(int orignNodeID) const
	{
		OrginNodeCoord orginNodeCoord = m_orignMap.getNodeCoord(orignNodeID);
		return getPartNodeID(toPartNodeCoord(orginNodeCoord));
	}	
}}}
