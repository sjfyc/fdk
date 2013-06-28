#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviBlockMap.h>

namespace fdk { namespace game { namespace navi
{
	void VertexMap::rebuildFromBlockMap()
	{
		m_data.reset(m_blockMap.getSizeX()*2-1, m_blockMap.getSizeY()*2-1);
		m_allowModify.reset(m_data.size_x(), m_data.size_y(), true);
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

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet)
	{
		for (UnitSize y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (UnitSize x = -m_unitSize; x <= m_unitSize; ++x)
			{				
				VertexCoord vertexCoord(cellCoord.x*2+x, cellCoord.y*2+y);
				if (isValidNodeCoord(vertexCoord))
				{
					m_data(vertexCoord.x, vertexCoord.y) += (bSet ? 1 : -1);
					FDK_ASSERT(m_data(vertexCoord.x, vertexCoord.y) >= 0);
				}
			}
		}		
	}

	int VertexMap::getHeuristic(int startNodeID, int targetNodeID) const
	{
		return getHeuristic(toNodeCoord(startNodeID), toNodeCoord(targetNodeID));
	}

	int VertexMap::getHeuristic(const VertexCoord& startVertexCoord, const VertexCoord& targetVertexCoord) const
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

	void VertexMap::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		VertexCoord coord = toNodeCoord(nodeID);

		const bool bLeft = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(navigator, result, VertexCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);

		if (bLeft || bTop)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bTop || bRight)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (bRight || bBottom)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (bBottom || bLeft)
		{
			tryAddSuccessorNode(navigator, result, VertexCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}
	}

	bool VertexMap::tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const VertexCoord& vertexCoord, int cost, int parentNodeID) const
	{
		if (!isValidNodeCoord(vertexCoord))
		{
			return false;
		}		
		const VertexID vertexID = toNodeID(vertexCoord);
		if (!isNodeReachable(vertexID))
		{
			return false;
		}

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

	bool VertexMap::isNodeReachable(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return m_data.raw_data()[nodeID] <= 0;
	}

	void VertexMap::onPlotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool bPlot)
	{
		UnitSize totalExtend = unitSize+m_unitSize-1;
		for (UnitSize y = -totalExtend; y <= totalExtend; ++y)
		{
			for (UnitSize x = -totalExtend; x <= totalExtend; ++x)
			{				
				VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
				if (!isValidNodeCoord(aroundVertexCoord))
				{
					continue;
				}

				if (!m_allowModify(aroundVertexCoord.x, aroundVertexCoord.y))
				{
					continue;
				}

				m_data(aroundVertexCoord.x, aroundVertexCoord.y) += (bPlot ? 1 : -1);
				FDK_ASSERT(m_data(aroundVertexCoord.x, aroundVertexCoord.y) >= 0);
			}
		}	
	}

	void VertexMap::allowModify(const VertexCoord& vertexCoord, UnitSize unitSize, bool bAllow)
	{
		UnitSize totalExtend = unitSize+m_unitSize;
		for (UnitSize y = -totalExtend; y <= totalExtend; ++y)
		{
			for (UnitSize x = -totalExtend; x <= totalExtend; ++x)
			{				
				VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
				if (isValidNodeCoord(aroundVertexCoord))
				{
					m_allowModify(aroundVertexCoord.x, aroundVertexCoord.y) = bAllow;
				}
			}
		}
	}
	
	int VertexMap::getSizeX() const
	{
		return (int)m_data.size_x();
	}

	int VertexMap::getSizeY() const
	{
		return (int)m_data.size_y();
	}
}}}
