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

	VertexID VertexMap::toVertexID(const VertexCoord& vertexCoord) const
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
	
	bool VertexMap::isValidVertexCoord(const VertexCoord& vertexCoord) const
	{
		return vertexCoord.x >= 0 && vertexCoord.y >= 0 && m_data.is_valid_index(vertexCoord.x, vertexCoord.y);
	}

	int VertexMap::getBlockValue(const VertexCoord& vertexCoord) const
	{
		return m_data(vertexCoord.x, vertexCoord.y);
	}
	
	bool VertexMap::isBlock(const VertexCoord& vertexCoord) const
	{
		return getBlockValue(vertexCoord) > 0;
	}

	void VertexMap::onSetBlock(const CellCoord& cellCoord, bool bSet)
	{
		for (UnitSize y = -m_unitSize; y <= m_unitSize; ++y)
		{
			for (UnitSize x = -m_unitSize; x <= m_unitSize; ++x)
			{				
				VertexCoord vertexCoord(cellCoord.x*2+x, cellCoord.y*2+y);
				if (isValidVertexCoord(vertexCoord))
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
		if (isBlock(vertexCoord))
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

	void VertexMap::onPlotUnit(const VertexCoord& vertexCoord, UnitSize unitSize, bool xAlign, bool yAlign, bool bPlot)
	{
		UnitSize totalExtend = unitSize+m_unitSize-1;
		for (UnitSize y = -totalExtend; y <= totalExtend+(yAlign?0:1); ++y)
		{
			for (UnitSize x = -totalExtend; x <= totalExtend+(xAlign?0:1); ++x)
			{				
				VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
				if (isValidVertexCoord(aroundVertexCoord))
				{
					m_data(aroundVertexCoord.x, aroundVertexCoord.y) += (bPlot ? 1 : -1);
					FDK_ASSERT(m_data(aroundVertexCoord.x, aroundVertexCoord.y) >= 0);
				}
			}
		}	
	}

	bool VertexMap::isDirectlyReachable(const VertexCoord& startVertexCoord, const VertexCoord& targetVertexCoord) const
	{
		int startX = startVertexCoord.x;
		int startY = startVertexCoord.y;
		int targetX = targetVertexCoord.x;
		int targetY = targetVertexCoord.y;

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
		for (int i = 1; i <= dx; ++i)
		{
			VertexCoord vertexCoord = VertexCoord(x, y);
			if (!isValidVertexCoord(vertexCoord) || isBlock(vertexCoord) )
			{
				return false;
			}

			if (NError >= 0)
			{
				if (state == DependOnY) 
				{
					vertexCoord.reset(x, y+sy);
					if (!isValidVertexCoord(vertexCoord) || isBlock(vertexCoord) )
					{
						return false;
					}

					x = x + sx;
					vertexCoord.reset(x, y);
					if (!isValidVertexCoord(vertexCoord) || isBlock(vertexCoord) )
					{
						return false;
					}

				}
				else
				{
					vertexCoord.reset(x+sx, y);
					if (!isValidVertexCoord(vertexCoord) || isBlock(vertexCoord) )
					{
						return false;
					}

					y = y + sy;
					vertexCoord.reset(x, y);
					if (!isValidVertexCoord(vertexCoord) || isBlock(vertexCoord) )
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

}}}
