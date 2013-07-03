#include <fdkgame/NaviBase.h>
#include <stack>
#include <fdkgame/NaviTypes.h>

namespace fdk { namespace game { namespace navi
{
	const GridEnv* Environment::toGridEnv() const
	{
		return 0;
	}

	GridEnv* Environment::toGridEnv()
	{
		return 0;
	}


	bool GridEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return nodeCoord.x >= 0 && nodeCoord.x < getSizeX()
			&& nodeCoord.y >= 0 && nodeCoord.y < getSizeY();
	}

	int GridEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}
	
	void GridEnv::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
	{
		getSuccessorNodes(navigator, nodeID, result, false);
	}

	inline bool cutCornerOp(bool bCutCorner, bool a, bool b)
	{
		return bCutCorner ? (a && b) : (a || b);
	}
	void GridEnv::getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result, bool bCutCorner) const
	{
		NodeCoord coord = toNodeCoord(nodeID);

		const bool bLeft = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y), COST_STRAIGHT, nodeID);		
		const bool bTop = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y-1), COST_STRAIGHT, nodeID);
		const bool bRight = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y), COST_STRAIGHT, nodeID);
		const bool bBottom = tryAddSuccessorNode(navigator, result, NodeCoord(coord.x,coord.y+1), COST_STRAIGHT, nodeID);

		if (cutCornerOp(bCutCorner, bLeft, bTop))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bTop, bRight))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y-1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bRight, bBottom))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x+1,coord.y+1), COST_DIAGONAL, nodeID);
		}
		if (cutCornerOp(bCutCorner, bBottom, bLeft))
		{
			tryAddSuccessorNode(navigator, result, NodeCoord(coord.x-1,coord.y+1), COST_DIAGONAL, nodeID);
		}
	}

	bool GridEnv::tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& nodeCoord, int cost, int parentNodeID) const
	{
		if (!isValidNodeCoord(nodeCoord))
		{
			return false;
		}		
		const int nodeID = toNodeID(nodeCoord);
		if (!isNodeReachable(nodeID))
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

	int GridEnv::getHeuristic(int startNodeID, int targetNodeID) const
	{
		return getHeuristicEuclidean(startNodeID, targetNodeID);
	}

	int GridEnv::getHeuristicManhattan(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		NodeCoord startToTarget = targetNodeCoord - startNodeCoord;
		return COST_STRAIGHT * (abs(startToTarget.x) + abs(startToTarget.y));
	}

	int GridEnv::getHeuristicChebyshev(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		return COST_STRAIGHT * maxOf(abs(startNodeCoord.x - targetNodeCoord.x), abs(startNodeCoord.y - targetNodeCoord.y));
	}

	int GridEnv::getHeuristicEuclidean(int startNodeID, int targetNodeID) const
	{
		NodeCoord startNodeCoord = toNodeCoord(startNodeID);
		NodeCoord targetNodeCoord = toNodeCoord(targetNodeID);

		NodeCoord startToTarget = targetNodeCoord - startNodeCoord;
		return COST_STRAIGHT * startToTarget.length();
	}
	
	const GridEnv* GridEnv::toGridEnv() const
	{
		return this;
	}

	GridEnv* GridEnv::toGridEnv()
	{
		return this;
	}
	
	GridEnv::GridEnv()
		: m_colorComponent(0)
	{
	}

	GridEnv::~GridEnv()
	{
		FDK_ASSERT(!m_colorComponent);
	}

	int GridPartEnv::getSizeX() const
	{
		return m_outer.getSizeX();
	}

	int GridPartEnv::getSizeY() const
	{
		return m_outer.getSizeY();
	}

	bool GridPartEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return m_range.contain(nodeCoord) && m_outer.isValidNodeCoord(nodeCoord);
	}

	bool GridPartEnv::isNodeReachable(int nodeID) const
	{
		return m_outer.isNodeReachable(nodeID);
	}

	GridEnvColorComponent::GridEnvColorComponent(const GridEnv& outer)
		: m_outer(outer)
	{
	}

	GridEnvColorComponent::~GridEnvColorComponent()
	{
	}

	void GridEnvColorComponent::refill()
	{
		const int sizeX = m_outer.getSizeX();
		const int sizeY = m_outer.getSizeY();
		m_colors.reset(sizeX, sizeY, UNCOLORED);
		
		ColorType color = 0;
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				CellCoord cur(x, y);
				if (m_outer.isNodeWithCoordReachable(cur) &&
					m_colors(x, y) == UNCOLORED)
				{
					floodFill(cur, color++);
				}
			}
		}
	}

	void GridEnvColorComponent::floodFill(const NodeCoord& nodeCoord, ColorType color)
	{
		std::stack<NodeCoord> pending;
		pending.push(nodeCoord);
		while (!pending.empty())
		{
			NodeCoord cur = pending.top();
			pending.pop();
			if (m_colors(cur.x, cur.y) != UNCOLORED)
			{
				continue;
			}
			
			m_colors(cur.x, cur.y) = color;

			const NodeCoord neighbours[] =
			{
				NodeCoord(cur.x-1, cur.y),
				NodeCoord(cur.x+1, cur.y),
				NodeCoord(cur.x, cur.y-1),
				NodeCoord(cur.x, cur.y+1),
			};

			for (size_t i = 0; i < FDK_DIM(neighbours); ++i)
			{
				const NodeCoord& neighbour = neighbours[i];
				if (m_outer.isValidNodeCoord(neighbour) &&
					m_outer.isNodeWithCoordReachable(neighbour) &&
					m_colors(neighbour.x, neighbour.y) == UNCOLORED)
				{
					pending.push(neighbour); // 斜角的节点可能被重复添加
				}
			}
		}
	}

}}}
