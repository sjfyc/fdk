#ifndef __FDKGAME_NAVIJPS_H_INCLUDE__
#define __FDKGAME_NAVIJPS_H_INCLUDE__
#include "NaviAStar.h"

namespace fdk { namespace game { namespace navi
{
	class FDKGAME_API Jps
		: public AStar
	{
		typedef AStar _Base;
	public:
		Jps(const GridBasedEnv& env, int startNodeID, int targetNodeID);
		virtual ~Jps();
		// AStar interfaces
		virtual void getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result);
	private:				
	};
}}}

#endif
