#include <fdkgame/NaviGridEnvUtil.h>
#include <fdkgame/NaviAStar.h>

namespace fdk { namespace game { namespace navi
{
	struct GridEnvReachEverywhereAdapter
		: public GridEnv
	{
		GridEnvReachEverywhereAdapter(const GridEnv& env) : m_env(env) {}
		virtual int getSizeX() const { return m_env.getSizeX(); }
		virtual int getSizeY() const { return m_env.getSizeY(); }
		virtual bool isNodeReachable(int nodeID) const { return m_env.isNodeReachable(nodeID); }
		virtual int getHeuristic(int startNodeID, int targetNodeID) const { return 0; }
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const
		{
			return m_env.getSuccessorNodesWithoutCheck(nodeID, result);
		}
		const GridEnv& m_env;
	};

	GridEnvColorComponent::ColorType getColor(const GridEnv& env, int nodeID)
	{
		return env.getColorComponent()
			? env.getColorComponent()->getColor(env.toNodeCoord(nodeID))
			: GridEnvColorComponent::UNCOLORED;
	}

	bool isDirectlyReachable(const GridEnv& env, int startNodeID, int targetNodeID)
	{
		typedef GridEnv::NodeCoord NodeCoord;

		if (startNodeID == targetNodeID)
		{
			return true;
		}

		const NodeCoord startNodeCoord = env.toNodeCoord(startNodeID);
		const NodeCoord targetNodeCoord = env.toNodeCoord(targetNodeID);

		int startX = startNodeCoord.x;
		int startY = startNodeCoord.y;
		int targetX = targetNodeCoord.x;
		int targetY = targetNodeCoord.y;

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
			NodeCoord nodeCoord = NodeCoord(x, y);
			if (!env.isValidNodeCoord(nodeCoord) || 
				!env.isNodeWithCoordReachable(nodeCoord) )
			{
				return false;
			}

			if (NError >= 0)
			{
				if (state == DependOnY) 
				{
					nodeCoord.reset(x, y+sy);
					if (!env.isValidNodeCoord(nodeCoord) || 
						!env.isNodeWithCoordReachable(nodeCoord) )
					{
						return false;
					}

					x = x + sx;
					nodeCoord.reset(x, y);
					if (!env.isValidNodeCoord(nodeCoord) || 
						!env.isNodeWithCoordReachable(nodeCoord) )
					{
						return false;
					}

				}
				else
				{
					nodeCoord.reset(x+sx, y);
					if (!env.isValidNodeCoord(nodeCoord) || 
						!env.isNodeWithCoordReachable(nodeCoord) )
					{
						return false;
					}

					y = y + sy;
					nodeCoord.reset(x, y);
					if (!env.isValidNodeCoord(nodeCoord) || 
						!env.isNodeWithCoordReachable(nodeCoord) )
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

	int getFirstDirectlyReachableNode(const GridEnv& env, int startNodeID, int targetNodeID)
	{
		typedef GridEnv::NodeCoord NodeCoord;

		if (startNodeID == targetNodeID)
		{
			return startNodeID;
		}

		const NodeCoord startNodeCoord = env.toNodeCoord(startNodeID);
		const NodeCoord targetNodeCoord = env.toNodeCoord(targetNodeID);

		int startX = startNodeCoord.x;
		int startY = startNodeCoord.y;
		int targetX = targetNodeCoord.x;
		int targetY = targetNodeCoord.y;

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
			NodeCoord nodeCoord = NodeCoord(x, y);
			if (env.isValidNodeCoord(nodeCoord) && 
				env.isNodeWithCoordReachable(nodeCoord) )
			{
				return env.toNodeID(nodeCoord);
			}

			if (NError >= 0)
			{
				if (state == DependOnY) 
				{
					nodeCoord.reset(x, y+sy);
					if (env.isValidNodeCoord(nodeCoord) && 
						env.isNodeWithCoordReachable(nodeCoord) )
					{
						return env.toNodeID(nodeCoord);
					}

					x = x + sx;
					nodeCoord.reset(x, y);
					if (env.isValidNodeCoord(nodeCoord) && 
						env.isNodeWithCoordReachable(nodeCoord) )
					{
						return env.toNodeID(nodeCoord);
					}
				}
				else
				{
					nodeCoord.reset(x+sx, y);
					if (env.isValidNodeCoord(nodeCoord) && 
						env.isNodeWithCoordReachable(nodeCoord) )
					{
						return env.toNodeID(nodeCoord);
					}

					y = y + sy;
					nodeCoord.reset(x, y);
					if (env.isValidNodeCoord(nodeCoord) && 
						env.isNodeWithCoordReachable(nodeCoord) )
					{
						return env.toNodeID(nodeCoord);
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
		return INVALID_NODEID;
	}
	
	int getFirstReachableNode(const GridEnv& env, int startNodeID, int targetNodeID)
	{
		GridEnvReachEverywhereAdapter envAdapter(env);

		struct CompleteCondition
			: public AStarCompleteCondition
		{
			virtual bool checkCondition(const Environment& l_env, int startNodeID, int targetNodeID, int nodeID) const
			{
				const GridEnv& env = static_cast<const GridEnv&>(l_env);
				return env.isNodeReachable(nodeID);
			}
		} completeCondition;

		AStar astar(envAdapter, startNodeID, targetNodeID, &completeCondition);
		if (astar.search() == AStar::SearchResult_PathUnexist)
		{
			return INVALID_NODEID;
		}
		return astar.getCompletedNodeID();
	}

	int getFirstSameColorNode(const GridEnv& env, int startNodeID, int targetNodeID)
	{
		GridEnvReachEverywhereAdapter envAdapter(env);

		struct CompleteCondition
			: public AStarCompleteCondition
		{
			CompleteCondition(const GridEnv& env, GridEnvColorComponent::ColorType targetColor)
				: m_env(env)
				, m_targetColor(targetColor)
			{
			}
			virtual bool checkCondition(const Environment& l_env, int startNodeID, int targetNodeID, int nodeID) const
			{
				const GridEnv& env = static_cast<const GridEnv&>(l_env);
				return env.isNodeReachable(nodeID) && getColor(m_env, nodeID) == m_targetColor;
			}
			const GridEnv& m_env;
			GridEnvColorComponent::ColorType m_targetColor;
		} completeCondition(env, getColor(env, targetNodeID));
		
		FDK_ASSERT(getColor(env, targetNodeID) != GridEnvColorComponent::UNCOLORED);

		AStar astar(envAdapter, startNodeID, targetNodeID, &completeCondition);
		if (astar.search() == AStar::SearchResult_PathUnexist)
		{
			return INVALID_NODEID;
		}
		return astar.getCompletedNodeID();
	}

	GridEnvOctPathPop::GridEnvOctPathPop(const GridEnv& env, const std::list<int>& path, bool bCopy)
		: m_env(env)
		, m_path(bCopy ? *new std::list<int>(path) : path)
		, m_bCopy(bCopy)
	{
	}

	GridEnvOctPathPop::~GridEnvOctPathPop()
	{
		if (m_bCopy)
		{
			delete &m_path;
		}
	}

	GridEnvOctPathPop::Direction GridEnvOctPathPop::getDirection(const NodeCoord& startNodeCoord, const NodeCoord& targetNodeCoord)
	{
		const NodeCoord startToTarget = targetNodeCoord - startNodeCoord;
		int dx = 0;
		if (startToTarget.x > 0)
		{
			dx = 1;
		}
		else if (startToTarget.x < 0)
		{
			dx = -1;
		}
		int dy = 0;
		if (startToTarget.y > 0)
		{
			dy = 1;
		}
		else if (startToTarget.y < 0)
		{
			dy = -1;
		}
		return Direction(dx, dy);
	}

	GridEnvOctPathPopEachNode::GridEnvOctPathPopEachNode(const GridEnv& env, const std::list<int>& path, bool bCopy)
		: _Base(env, path, bCopy)
		, m_it(m_path.begin())
		, m_currentNodeCoord()
		, m_nextDirection()
	{			
	}

	GridEnvOctPathPopEachNode::~GridEnvOctPathPopEachNode()
	{
	}

	int GridEnvOctPathPopEachNode::pop()
	{
		FDK_ASSERT(!empty());

		bool bReachPort = false;

		if (m_it == m_path.begin())
		{
			m_currentNodeCoord = m_env.toNodeCoord(*m_it);
			bReachPort = true;
		}
		else
		{
			m_currentNodeCoord += m_nextDirection;
			if (m_currentNodeCoord == m_nextPortNodeCoord)
			{
				bReachPort = true;
			}
		}
		
		if (bReachPort)
		{
			++m_it;
			if (m_it != m_path.end())
			{
				m_nextPortNodeCoord = m_env.toNodeCoord(*m_it);
				m_nextDirection = getDirection(m_currentNodeCoord, m_nextPortNodeCoord);
			}
		}		

		return m_env.toNodeID(m_currentNodeCoord);
	}

	bool GridEnvOctPathPopEachNode::empty() const
	{
		return m_it == m_path.end();
	}

	GridEnvOctPathPopDirectlyReachableNode::GridEnvOctPathPopDirectlyReachableNode(const GridEnv& env, const std::list<int>& path, bool bCopy)
		: _Base(env, path, bCopy)
		, m_it(m_path.begin())
		, m_currentNodeID(INVALID_NODEID)
	{
	}

	GridEnvOctPathPopDirectlyReachableNode::~GridEnvOctPathPopDirectlyReachableNode()
	{
	}

	int GridEnvOctPathPopDirectlyReachableNode::pop()
	{
		FDK_ASSERT(!empty());

		if (m_it == m_path.begin())
		{
			m_currentNodeID = *m_it;
			++m_it;
			return m_currentNodeID;
		}
				
		int nextNodeID = *m_it;
		if (!isDirectlyReachable(m_env, m_currentNodeID, nextNodeID))
		{
			m_currentNodeID = nextNodeID;
			++m_it;
			return m_currentNodeID;
		}

		int initNodeID = m_currentNodeID;
		while (1)
		{
			m_currentNodeID = nextNodeID;
			++m_it;
			if (m_it == m_path.end())
			{
				break;
			}
			nextNodeID = *m_it;
			if (!isDirectlyReachable(m_env, initNodeID, nextNodeID))
			{
				break;
			}
		}

		return m_currentNodeID;
	}

	bool GridEnvOctPathPopDirectlyReachableNode::empty() const
	{
		return m_it == m_path.end();
	}
}}}
