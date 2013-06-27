#include <fdkgame/NaviJps.h>
#include <fdkgame/NaviTypes.h>

namespace fdk { namespace game { namespace navi
{
	typedef GridBasedEnv::NodeCoord NodeCoord;

	class JpsUtil
	{
	public:
		enum { NO_DIRECTION = 8 };
		enum { EMPTY_DIRECTIONS = 0, FULL_DIRECTIONS = 255 } ;
		typedef unsigned char Direction;
		typedef unsigned char Directions;
		typedef GridBasedEnv::NodeCoord NodeCoord;
		static bool isDiagonalDirection(Direction direction);
		static Direction getNextDirection(Directions& directions);
		static Direction getDirectionFromParent(const NodeCoord* parentNodeCoord, const NodeCoord& nodeCoord);
		static NodeCoord getNeighbourNodeCoordInDirection(const NodeCoord& nodeCoord, Direction direction);
		static Directions getNaturalNeighbourDirections(Direction direction);
		static Directions getForcedNeighbourDirections(const GridBasedEnv& env, const NodeCoord& nodeCoord, Direction direction);
		static bool isNeighbourInDirectionReachable(const GridBasedEnv& env, const NodeCoord& nodeCoord, Direction direction);
		static int jump(const GridBasedEnv& env, int targetNodeID, const NodeCoord& nodeCoord, Direction direction);
	};

	Jps::Jps(const GridBasedEnv& env, int startNodeID, int targetNodeID)
		: _Base(env, startNodeID, targetNodeID)
	{
	}

	Jps::~Jps()
	{
	}

	void Jps::getSuccessorNodes(const Environment& l_env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result)
	{
		const GridBasedEnv& env = static_cast<const GridBasedEnv&>(l_env);
		GridBasedEnv::NodeCoord nodeCoord = env.toNodeCoord(nodeID);
		GridBasedEnv::NodeCoord* pParentNodeCoord = 0;
		GridBasedEnv::NodeCoord parentNodeCoord;
		if (parentNodeID != INVALID_NODEID)
		{
			parentNodeCoord = env.toNodeCoord(parentNodeID);
			pParentNodeCoord = &parentNodeCoord;
		}

		JpsUtil::Direction fromParentDirection = JpsUtil::getDirectionFromParent(pParentNodeCoord, nodeCoord);
		JpsUtil::Directions naturalNeighbourDirections = JpsUtil::getNaturalNeighbourDirections(fromParentDirection);		
		JpsUtil::Directions forcedNeighbourDirections = JpsUtil::getForcedNeighbourDirections(env, nodeCoord, fromParentDirection);
		JpsUtil::Directions directions = naturalNeighbourDirections | forcedNeighbourDirections;

		for (JpsUtil::Direction direction = JpsUtil::getNextDirection(directions); 
			direction != JpsUtil::NO_DIRECTION; direction = JpsUtil::getNextDirection(directions))
		{			
			int successorNodeID = JpsUtil::jump(env, getTargetNodeID(), nodeCoord, direction);
			if (successorNodeID == INVALID_NODEID)
			{
				continue;
			}
			GridBasedEnv::NodeCoord successorNodeCoord = env.toNodeCoord(successorNodeID);			
			GridBasedEnv::NodeCoord offset = successorNodeCoord-nodeCoord;
			SuccessorNodeInfo info;
			info.nodeID = successorNodeID;
			
			if (JpsUtil::isDiagonalDirection(direction))
			{
				FDK_ASSERT(abs(offset.x) == abs(offset.y));
				info.cost = COST_DIAGONAL * abs(offset.x);
			}
			else
			{
				info.cost = COST_STRAIGHT * abs(offset.x != 0 ? offset.x : offset.y);
			}
			result.push_back(info);
		}
	}

	bool JpsUtil::isDiagonalDirection(Direction direction)
	{
		return (direction % 2) != 0;
	}

	JpsUtil::Direction JpsUtil::getNextDirection(Directions& directions)
	{
		Direction dir;
		for (dir = 0 ; dir < 8 ; dir++) 
		{
			char dirBit = 1 << dir;
			if (directions & dirBit) 
			{
				directions ^= dirBit;
				return dir;
			}
		}
		return NO_DIRECTION;
	}

	JpsUtil::Direction JpsUtil::getDirectionFromParent(const NodeCoord* parentNodeCoord, const NodeCoord& nodeCoord)
	{	
		// top:0 topRight:1 ... clockwise
		if (!parentNodeCoord)
		{
			return NO_DIRECTION;
		}
		const NodeCoord offset = nodeCoord - *parentNodeCoord;
		if (offset.x == 0 && offset.y < 0) 
		{
			return 0;
		} 
		else if (offset.x > 0 && offset.y < 0) 
		{
			return 1;
		} 
		else if (offset.x > 0 && offset.y == 0) 
		{
			return 2;
		} 
		else if (offset.x > 0 && offset.y > 0) 
		{
			return 3;
		} 
		else if (offset.x == 0 && offset.y > 0)
		{
			return 4;
		} 
		else if (offset.x < 0 && offset.y > 0) 
		{
			return 5;
		} 
		else if (offset.x < 0 && offset.y == 0) 
		{
			return 6;
		} 
		else if (offset.x < 0 && offset.y < 0)
		{
			return 7;
		}
		else
		{
			return NO_DIRECTION;
		}		
	}

	JpsUtil::NodeCoord JpsUtil::getNeighbourNodeCoordInDirection(const NodeCoord& nodeCoord, Direction direction)
	{
		switch (direction % 8)
		{
		case 0: return NodeCoord(nodeCoord.x, nodeCoord.y - 1);
		case 1: return NodeCoord(nodeCoord.x + 1, nodeCoord.y - 1);
		case 2: return NodeCoord(nodeCoord.x + 1, nodeCoord.y);
		case 3: return NodeCoord(nodeCoord.x + 1, nodeCoord.y + 1);
		case 4: return NodeCoord(nodeCoord.x, nodeCoord.y + 1);
		case 5: return NodeCoord(nodeCoord.x - 1, nodeCoord.y + 1);
		case 6: return NodeCoord(nodeCoord.x - 1, nodeCoord.y);
		case 7: return NodeCoord(nodeCoord.x - 1, nodeCoord.y - 1);
		default: return NodeCoord(-1, -1);
		}
	}

	JpsUtil::Directions JpsUtil::getNaturalNeighbourDirections(Direction direction)
	{
		if (direction == NO_DIRECTION)
		{
			return FULL_DIRECTIONS;
		}

		Directions directions = EMPTY_DIRECTIONS;		
		setEnumMask(directions, direction);

		if (isDiagonalDirection(direction))
		{
			setEnumMask(directions, (direction + 1) % 8);
			setEnumMask(directions, (direction + 7) % 8);
		}

		return directions;
	}
	
	JpsUtil::Directions JpsUtil::getForcedNeighbourDirections(const GridBasedEnv& env, const NodeCoord& nodeCoord, Direction direction)
	{
		if (direction == NO_DIRECTION) 
		{
			return EMPTY_DIRECTIONS;
		}

		Directions directions = EMPTY_DIRECTIONS;
		
		if (isDiagonalDirection(direction)) 
		{
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction+6) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction+5))
			{
				setEnumMask(directions, (direction + 6) % 8);
			}
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +2) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +3)) 
			{
				setEnumMask(directions, (direction + 2) % 8);
			}
		} 
		else 
		{
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +1) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +2)) 
			{
				setEnumMask(directions, (direction + 1) % 8);
			}
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +7) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +6)) 
			{
				setEnumMask(directions, (direction + 7) % 8);
			}
		}

		return directions;
	}

	bool JpsUtil::isNeighbourInDirectionReachable(const GridBasedEnv& env, const NodeCoord& nodeCoord, Direction direction)
	{
		if (isDiagonalDirection(direction))
		{
			if (!isNeighbourInDirectionReachable(env, nodeCoord, (direction + 7) % 8) &&
				!isNeighbourInDirectionReachable(env, nodeCoord, (direction + 1) % 8))
			{
				return false;
			}
		}
		NodeCoord neighbourNodeCoord = getNeighbourNodeCoordInDirection(nodeCoord, direction % 8);
		if (!env.isValidNodeCoord(neighbourNodeCoord))
		{
			return false;
		}
		return env.isNodeReachable(env.toNodeID(neighbourNodeCoord));		
	}

	int JpsUtil::jump(const GridBasedEnv& env, int targetNodeID, const NodeCoord& nodeCoord, Direction direction)
	{
		if (!isNeighbourInDirectionReachable(env, nodeCoord, direction))
		{
			return INVALID_NODEID;
		}
		
		NodeCoord stepNodeCoord = getNeighbourNodeCoordInDirection(nodeCoord, direction);		
		int stepNodeID = env.toNodeID(stepNodeCoord);

		if (stepNodeID == targetNodeID) 
		{
			return stepNodeID;
		}
		if (getForcedNeighbourDirections(env, stepNodeCoord, direction) != EMPTY_DIRECTIONS) 
		{
			return stepNodeID;
		}
		if (isDiagonalDirection(direction)) 
		{
			int nodeID = jump(env, targetNodeID, stepNodeCoord, (direction + 7) % 8);
			if (nodeID != INVALID_NODEID) 
			{
				return stepNodeID;
			}
			nodeID = jump(env, targetNodeID, stepNodeCoord, (direction + 1) % 8);
			if (nodeID != INVALID_NODEID) 
			{
				return stepNodeID;
			}			
		}
		return jump(env, targetNodeID, stepNodeCoord, direction);
	}

}}}
