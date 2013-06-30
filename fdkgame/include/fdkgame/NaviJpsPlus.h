#ifndef __FDKGAME_NAVIJPSPLUS_H_INCLUDE__
#define __FDKGAME_NAVIJPSPLUS_H_INCLUDE__
#include "NaviAStar.h"

namespace fdk { namespace game { namespace navi
{
	class JpsPlusImpl;

	class FDKGAME_API JpsPlus
		: public AStar
	{
		typedef AStar _Base;
	public:
		JpsPlus(const GridEnv& env, int startNodeID, int targetNodeID, std::list<int>& outputPath);
		virtual ~JpsPlus();
		// AStar interfaces
		virtual void getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result);		
	};


}}}

#endif
