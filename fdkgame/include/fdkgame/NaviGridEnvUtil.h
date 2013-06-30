#ifndef __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#define __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#include "NaviBase.h"

namespace fdk { namespace game { namespace navi
{
	bool isDirectlyReachable(const GridEnv& env, int startNodeID, int targetNodeID);
	int getFirstDirectlyReachableNode(const GridEnv& env, int startNodeID, int targetNodeID);

	class GridEnvOctPathPop
	{
	public:

	};
}}}

#endif
