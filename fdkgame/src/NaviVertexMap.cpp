#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviBlockMap.h>

namespace fdk { namespace game { namespace navi
{
	VertexMap::VertexMap(BlockMap& blockMap, UnitSize unitSize)
		: m_blockMap(blockMap)
		, m_unitSize(unitSize)
	{
		FDK_ASSERT(unitSize >= 1);
	}

	VertexMap::~VertexMap()
	{
	}

	void VertexMap::rebuildFromBlockMap()
	{
		m_data.reset(m_blockMap.getSizeX()*2-1, m_blockMap.getSizeY()*2-1);
		for (int y = 0; y < (int)m_blockMap.getSizeY(); ++y)
		{
			for (int x = 0; x < (int)m_blockMap.getSizeX(); ++x)
			{
				CellCoord cellCoord = CellCoord(x, y);
				if (m_blockMap.isBlock(cellCoord))
				{
					onSetBlock(cellCoord, true);
				}
			}
		}
	}

	size_t VertexMap::getSizeX() const
	{
		return m_data.size_x();
	}

	size_t VertexMap::getSizeY() const
	{
		return m_data.size_y();
	}

	VertexID VertexMap::toVertexID(const VertexCoord vertexCoord) const
	{
		FDK_ASSERT(isValidVertexCoord(vertexCoord));
		return m_data.to_index(vertexCoord.x, vertexCoord.y);
	}

	VertexCoord VertexMap::toVertexCoord(VertexID vertexID) const
	{
		FDK_ASSERT(isValidVertexID(vertexID));
		MapData::size_type x, y;
		m_data.to_index(vertexID, x, y);
		return VertexCoord(x, y);
	}

	bool VertexMap::isValidVertexID(VertexID vertexID) const
	{
		return vertexID >= 0 && m_data.is_valid_index(vertexID);	
	}
	bool VertexMap::isValidVertexCoord(const VertexCoord vertexCoord) const
	{
		return vertexCoord.x >= 0 && vertexCoord.y >= 0 && m_data.is_valid_index(vertexCoord.x, vertexCoord.y);
	}

	int VertexMap::getBlockValue(const CellCoord& cellCoord) const
	{
		return m_data(cellCoord.x, cellCoord.y);
	}
	
	bool VertexMap::isBlock(const CellCoord& cellCoord) const
	{
		return getBlockValue(cellCoord) > 0;
	}

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet)
	{
		for (UnitSize y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (UnitSize x = -m_unitSize; x <= m_unitSize; ++x)
			{				
				VertexCoord vertexCoord(cellCoord.x*2+x, cellCoord.y*2+y);
				if (m_data.is_valid_index(vertexCoord.x, vertexCoord.y))
				{
					m_data(vertexCoord.x, vertexCoord.y) += (bSet ? 1 : -1);
					FDK_ASSERT(m_data(vertexCoord.x, vertexCoord.y) >= 0);
				}
			}
		}		
	}	
		
	int VertexMap::getNodeSpaceSize() const
	{
		return m_data.count();
	}

	int VertexMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		VertexCoord startToTarget = toVertexCoord(startNodeID) - toVertexCoord(startNodeID);
		return (abs(startToTarget.x) + abs(startToTarget.y)) * COST_STRAIGHT;
	}

	void VertexMap::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		VertexCoord coord = toVertexCoord(nodeID);

		const bool bLeft = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);

		if (bLeft && bTop)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bTop && bRight)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bRight && bBottom)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (bBottom && bLeft)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}
	}

	bool VertexMap::tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const VertexCoord& vertexCoord, int cost, int parentNodeID) const
	{
		if (!isValidVertexCoord(vertexCoord))
		{
			return false;
		}
		if (!isBlock(vertexCoord))
		{
			return false;
		}
		const VertexID vertexID = toVertexID(vertexCoord);
		
		//if (navigator.getEnvironmentChecker() &&
		//	!navigator.getEnvironmentChecker()->checkSuccessorNode(*this, nodeID, parentNodeID))
		//{
		//	return false;
		//}

		SuccessorNodeInfo info;
		info.nodeID = vertexID;
		info.cost = cost;
		result.push_back(info);
		return true;
	}
}}}
