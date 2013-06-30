#include <fdkgame/NaviJpsPlus.h>
#include <fdkgame/NaviTypes.h>

namespace fdk { namespace game { namespace navi
{
	typedef GridEnv::NodeCoord NodeCoord;

	class JpsPlusDef
	{
	public:
		enum { MAX_DIRECTION_COUNT = 8 };
		enum { NO_DIRECTION = 8 };
		enum { EMPTY_DIRECTIONS = 0, FULL_DIRECTIONS = 255 } ;
		typedef unsigned char Direction;
		typedef unsigned char Directions;
		typedef GridEnv::NodeCoord NodeCoord;
	};

	class JpsPlusUtil
		: public JpsPlusDef
	{
	public:
		
		static bool isDiagonalDirection(Direction direction);
		static Direction getNextDirection(Directions& directions);
		static Direction getDirectionFromParent(const NodeCoord* parentNodeCoord, const NodeCoord& nodeCoord);
		static NodeCoord getNeighbourNodeCoordInDirection(const NodeCoord& nodeCoord, Direction direction);
		static Directions getNaturalNeighbourDirections(Direction direction);
		static Directions getForcedNeighbourDirections(const GridEnv& env, const NodeCoord& nodeCoord, Direction direction);
		static bool isNeighbourInDirectionReachable(const GridEnv& env, const NodeCoord& nodeCoord, Direction direction, bool bDiagonal);
		static int jump(const GridEnv& env, int targetNodeID, const NodeCoord& nodeCoord, Direction direction);
	};

	
	JpsPlus::JpsPlus(const GridEnv& env, int startNodeID, int targetNodeID, std::list<int>& outputPath)
		: _Base(env, startNodeID, targetNodeID, outputPath)
	{
	}

	JpsPlus::~JpsPlus()
	{
	}

	void JpsPlus::getSuccessorNodes(const Environment& l_env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result)
	{
		const GridEnv& env = static_cast<const GridEnv&>(l_env);
		GridEnv::NodeCoord nodeCoord = env.toNodeCoord(nodeID);
		GridEnv::NodeCoord* pParentNodeCoord = 0;
		GridEnv::NodeCoord parentNodeCoord;
		if (parentNodeID != INVALID_NODEID)
		{
			parentNodeCoord = env.toNodeCoord(parentNodeID);
			pParentNodeCoord = &parentNodeCoord;
		}

		JpsPlusUtil::Direction fromParentDirection = JpsPlusUtil::getDirectionFromParent(pParentNodeCoord, nodeCoord);
		JpsPlusUtil::Directions naturalNeighbourDirections = JpsPlusUtil::getNaturalNeighbourDirections(fromParentDirection);		
		JpsPlusUtil::Directions forcedNeighbourDirections = JpsPlusUtil::getForcedNeighbourDirections(env, nodeCoord, fromParentDirection);
		JpsPlusUtil::Directions directions = naturalNeighbourDirections | forcedNeighbourDirections;
		
		for (JpsPlusUtil::Direction direction = JpsPlusUtil::getNextDirection(directions); 
			direction != JpsPlusUtil::NO_DIRECTION; direction = JpsPlusUtil::getNextDirection(directions))
		{
			int successorNodeID = JpsPlusUtil::jump(env, getTargetNodeID(), nodeCoord, direction);
			if (successorNodeID == INVALID_NODEID)
			{
				continue;
			}
			GridEnv::NodeCoord successorNodeCoord = env.toNodeCoord(successorNodeID);
			GridEnv::NodeCoord offset = successorNodeCoord-nodeCoord;
			SuccessorNodeInfo info;
			info.nodeID = successorNodeID;
			
			if (JpsPlusUtil::isDiagonalDirection(direction))
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
	
	inline bool JpsPlusUtil::isDiagonalDirection(Direction direction)
	{
		return (direction % 2) != 0;
	}

	JpsPlusUtil::Direction JpsPlusUtil::getNextDirection(Directions& directions)
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

	JpsPlusUtil::Direction JpsPlusUtil::getDirectionFromParent(const NodeCoord* parentNodeCoord, const NodeCoord& nodeCoord)
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

	JpsPlusUtil::NodeCoord JpsPlusUtil::getNeighbourNodeCoordInDirection(const NodeCoord& nodeCoord, Direction direction)
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

	JpsPlusUtil::Directions JpsPlusUtil::getNaturalNeighbourDirections(Direction direction)
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
	
	JpsPlusUtil::Directions JpsPlusUtil::getForcedNeighbourDirections(const GridEnv& env, const NodeCoord& nodeCoord, Direction direction)
	{
		if (direction == NO_DIRECTION) 
		{
			return EMPTY_DIRECTIONS;
		}

		Directions directions = EMPTY_DIRECTIONS;
		
		if (isDiagonalDirection(direction)) 
		{
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction+6, true) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction+5, false))
			{
				setEnumMask(directions, (direction + 6) % 8);
			}
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +2, true) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +3, false)) 
			{
				setEnumMask(directions, (direction + 2) % 8);
			}
		} 
		else 
		{
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +1, true) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +2, false)) 
			{
				setEnumMask(directions, (direction + 1) % 8);
			}
			if (isNeighbourInDirectionReachable(env, nodeCoord, direction +7, true) && 
				!isNeighbourInDirectionReachable(env, nodeCoord, direction +6, false)) 
			{
				setEnumMask(directions, (direction + 7) % 8);
			}
		}

		return directions;
	}

	bool JpsPlusUtil::isNeighbourInDirectionReachable(const GridEnv& env, const NodeCoord& nodeCoord, Direction direction, bool bDiagonal)
	{
		if (bDiagonal)
		{
			if (!isNeighbourInDirectionReachable(env, nodeCoord, (direction + 7) % 8, false) &&
				!isNeighbourInDirectionReachable(env, nodeCoord, (direction + 1) % 8, false))
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

	int JpsPlusUtil::jump(const GridEnv& env, int targetNodeID, const NodeCoord& nodeCoord, Direction direction)
	{
		const bool bDiagonal = isDiagonalDirection(direction);

		if (!isNeighbourInDirectionReachable(env, nodeCoord, direction, bDiagonal))
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

		if (bDiagonal)
		{
			Direction newDirs[] = { (direction + 7) % 8, (direction + 1) % 8 }; 
			for (size_t i = 0; i < sizeof(newDirs)/sizeof(*newDirs); ++i)
			{
				if (jump(env, targetNodeID, stepNodeCoord, newDirs[i]) != INVALID_NODEID)
				{
					return stepNodeID;
				}
			}	
		}
		return jump(env, targetNodeID, stepNodeCoord, direction);
	}
}}}
