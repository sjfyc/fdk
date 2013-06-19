#include <fdkgame/FindPathGridMap.h>
#include <math.h>

namespace fdk { namespace game { namespace findpath
{
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

	int GridMap::getNodeSpaceSize() const
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
		NodeCoord coord = getNodeCoord(nodeID);
	
		const bool bLeft = tryAddSuccessorNode(result, NodeCoord(coord.x-1,coord.y), COST_STRAIGHT);		
		const bool bTop = tryAddSuccessorNode(result, NodeCoord(coord.x,coord.y-1), COST_STRAIGHT);
		const bool bRight = tryAddSuccessorNode(result, NodeCoord(coord.x+1,coord.y), COST_STRAIGHT);
		const bool bBottom = tryAddSuccessorNode(result, NodeCoord(coord.x,coord.y+1), COST_STRAIGHT);
		
		// 横向纵向至少有一个可以展开才考虑斜向（两个都考虑就是别马脚算法）
		if (bLeft || bTop)
		{
			tryAddSuccessorNode(result, NodeCoord(coord.x-1,coord.y-1), COST_DIAGONAL);
		}
		if (bTop || bRight)
		{
			tryAddSuccessorNode(result, NodeCoord(coord.x+1,coord.y-1), COST_DIAGONAL);
		}
		if (bRight || bBottom)
		{
			tryAddSuccessorNode(result, NodeCoord(coord.x+1,coord.y+1), COST_DIAGONAL);
		}
		if (bBottom || bLeft)
		{
			tryAddSuccessorNode(result, NodeCoord(coord.x-1,coord.y+1), COST_DIAGONAL);
		}		
	}
	
	bool GridMap::tryAddSuccessorNode(std::vector<SuccessorNodeInfo>& result, const NodeCoord& coord, int cost) const
	{
		const int nodeID = getNodeID(coord);
		if (nodeID == INVALID_NODEID)
		{
			return false;
		}
		if (isObstacle(nodeID))
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
		return m_nodes.raw_data()[nodeID].bObstacle;
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
		const NodeCoord bottomNodeCoord(coord.x, coord.y+1);
		const NodeCoord rightNodeCoord(coord.x+1, coord.y);		
		const NodeCoord bottomRightNodeCoord(coord.x+1, coord.y+1);
		if (isValidNodeCoord(rightNodeCoord) && 
			isValidNodeCoord(bottomNodeCoord) && 
			isValidNodeCoord(bottomRightNodeCoord) )
		{
			int temp = minOf(				
				m_nodes(bottomNodeCoord.x, bottomNodeCoord.y).clearanceValue,
				m_nodes(rightNodeCoord.x, rightNodeCoord.y).clearanceValue
				);
			m_nodes(coord.x, coord.y).clearanceValue = minOf(
				temp, 
				m_nodes(bottomRightNodeCoord.x, bottomRightNodeCoord.y).clearanceValue
				);
		}
		else
		{
			m_nodes(coord.x, coord.y).clearanceValue = 1;
		}
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

	void GridMapPart::getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		int orignNodeID = toOrignNodeID(nodeID);
		std::vector<SuccessorNodeInfo> temp;
		m_orignMap.getSuccessorNodes(orignNodeID, temp);
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
