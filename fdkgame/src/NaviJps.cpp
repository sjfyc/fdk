#include <fdkgame/NaviJps.h>
#include <fdkgame/NaviTypes.h>

namespace fdk { namespace game { namespace navi
{
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
		static Direction getDirectionFromParentToNode(int mapWidth, int parentNodeID, int nodeID);
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

	void Jps::getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result)
	{
		const GridBasedEnv& gridBasedEnv = static_cast<const GridBasedEnv&>(env);
		GridBasedEnv::NodeCoord nodeCoord = gridBasedEnv.toNodeCoord(nodeID);
		JpsUtil::Direction parentToCurrentDirection = JpsUtil::getDirectionFromParentToNode(gridBasedEnv.getSizeX(), parentNodeID, nodeID);
		JpsUtil::Directions naturalNeighbourDirections = JpsUtil::getNaturalNeighbourDirections(parentToCurrentDirection);
		JpsUtil::Directions forcedNeighbourDirections = JpsUtil::getForcedNeighbourDirections(gridBasedEnv, nodeCoord, parentToCurrentDirection);
		JpsUtil::Directions directions = naturalNeighbourDirections | forcedNeighbourDirections;
		for (JpsUtil::Direction direction = JpsUtil::getNextDirection(directions); 
			direction != JpsUtil::NO_DIRECTION; direction = JpsUtil::getNextDirection(directions))
		{			
			int successorNodeID = JpsUtil::jump(gridBasedEnv, getTargetNodeID(), nodeCoord, direction);
			if (successorNodeID == INVALID_NODEID)
			{
				continue;
			}
			GridBasedEnv::NodeCoord successorNodeCoord = gridBasedEnv.toNodeCoord(successorNodeID);
			SuccessorNodeInfo info;
			info.nodeID = successorNodeID;
			info.cost = COST_STRAIGHT * (successorNodeCoord-nodeCoord).length();
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

	JpsUtil::Direction JpsUtil::getDirectionFromParentToNode(int mapWidth, int parentNodeID, int nodeID)
	{	
		// top:0 topRight:1 ... clockwise
		if (parentNodeID - mapWidth == nodeID) 
		{
			return 0;
		} 
		else if (parentNodeID - mapWidth + 1 == nodeID) 
		{
			return 1;
		} 
		else if (parentNodeID + 1 == nodeID) 
		{
			return 2;
		} 
		else if (parentNodeID + mapWidth + 1 == nodeID) 
		{
			return 3;
		} 
		else if (parentNodeID + mapWidth == nodeID) {
			return 4;
		} 
		else if (parentNodeID + mapWidth - 1 == nodeID) 
		{
			return 5;
		} 
		else if (parentNodeID - 1 == nodeID) 
		{
			return 6;
		} 
		else if (parentNodeID - mapWidth - 1 == nodeID) 
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
		NodeCoord neighbourNodeCoord = getNeighbourNodeCoordInDirection(nodeCoord, direction % 8);
		if (!env.isValidNodeCoord(neighbourNodeCoord))
		{
			return false;
		}
		return env.isNodeReachable(env.toNodeID(neighbourNodeCoord));
	}

	int JpsUtil::jump(const GridBasedEnv& env, int targetNodeID, const NodeCoord& nodeCoord, Direction direction)
	{
		NodeCoord stepNodeCoord = getNeighbourNodeCoordInDirection(nodeCoord, direction);
		if (!env.isValidNodeCoord(stepNodeCoord))
		{
			return INVALID_NODEID;
		}
		int stepNodeID = env.toNodeID(stepNodeCoord);
		if (!env.isNodeReachable(stepNodeID)) 
		{
			return INVALID_NODEID;
		}
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
