#ifndef __FDKGAME_FINDPATH_FINDPATHENV_H_INCLUDE__
#define __FDKGAME_FINDPATH_FINDPATHENV_H_INCLUDE__
#include <fdkgame/Base.h>
#include <vector>

namespace fdk { namespace game 
{
	class FindPathEnv
	{
	public:
		struct SuccessorNodeInfo
		{
			int nodeID;
			int cost;
		};
		virtual ~FindPathEnv() {}		
		virtual int getHeuristic(int startNodeID, int targetNodeID) const = 0;
		virtual void getSuccessors(int nodeId, vector<SuccessorNodeInfo>& result) const = 0;
	};
}}

#endif
