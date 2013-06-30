#include <fdkgame/NaviGridEnvUtil.h>

namespace fdk { namespace game { namespace navi
{
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


	GridEnvOctPathPop::GridEnvOctPathPop(const GridEnv& env, const std::vector<int>& path, bool bCopy)
		: m_env(env)
		, m_path(bCopy ? *new std::vector<int>(path) : path)
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

	GridEnvOctPathPopEachNode::GridEnvOctPathPopEachNode(const GridEnv& env, const std::vector<int>& path, bool bCopy)
		: _Base(env, path, bCopy)
		, m_index(path.empty() ? -1 : (path.size()-1) )
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

		if (m_index == m_path.size()-1)
		{
			m_currentNodeCoord = m_env.toNodeCoord(m_path[m_index]);
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
			--m_index;
			if (m_index != -1)
			{
				m_nextPortNodeCoord = m_env.toNodeCoord(m_path[m_index]);
				m_nextDirection = getDirection(m_currentNodeCoord, m_nextPortNodeCoord);
			}
		}		

		return m_env.toNodeID(m_currentNodeCoord);
	}

	bool GridEnvOctPathPopEachNode::empty() const
	{
		return m_index == -1;
	}

	GridEnvOctPathPopDirectlyReachableNode::GridEnvOctPathPopDirectlyReachableNode(const GridEnv& env, const std::vector<int>& path, bool bCopy)
		: _Base(env, path, bCopy)
		, m_index(path.empty() ? -1 : (path.size()-1) )
		, m_currentNodeID(INVALID_NODEID)
	{
	}

	GridEnvOctPathPopDirectlyReachableNode::~GridEnvOctPathPopDirectlyReachableNode()
	{
	}

	int GridEnvOctPathPopDirectlyReachableNode::pop()
	{
		FDK_ASSERT(!empty());

		if (m_index == m_path.size()-1)
		{
			m_currentNodeID = m_path[m_index];
			--m_index;
			return m_currentNodeID;
		}
				
		int nextNodeID = m_path[m_index];
		if (!isDirectlyReachable(m_env, m_currentNodeID, nextNodeID))
		{
			m_currentNodeID = nextNodeID;
			--m_index;
			return m_currentNodeID;
		}

		while (1)
		{
			m_currentNodeID = nextNodeID;
			--m_index;
			if (m_index == -1)
			{
				break;
			}
			nextNodeID = m_path[m_index];
			if (!isDirectlyReachable(m_env, m_currentNodeID, nextNodeID))
			{
				break;
			}
		}

		return m_currentNodeID;
	}

	bool GridEnvOctPathPopDirectlyReachableNode::empty() const
	{
		return m_index == -1;
	}
}}}
