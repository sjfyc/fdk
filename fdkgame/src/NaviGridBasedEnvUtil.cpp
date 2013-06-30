#include <fdkgame/NaviGridBasedEnvUtil.h>

namespace fdk { namespace game { namespace navi
{
	typedef GridBasedEnv::NodeCoord NodeCoord;

	bool isDirectlyReachable(const GridBasedEnv& env, int startNodeID, int targetNodeID)
	{
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

	int getFirstDirectlyReachableNode(const GridBasedEnv& env, int startNodeID, int targetNodeID)
	{
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

}}}
