#include <fdkgame/NaviBase.h>
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
}}}
