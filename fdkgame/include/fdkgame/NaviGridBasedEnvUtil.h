#ifndef __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#define __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#include "NaviBase.h"

namespace fdk { namespace game { namespace navi
{
	bool isDirectlyReachable(const GridBasedEnv& env, int startNodeID, int targetNodeID);
	int getFirstDirectlyReachableNode(const GridBasedEnv& env, int startNodeID, int targetNodeID);
}}}

#endif
