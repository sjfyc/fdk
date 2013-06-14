#ifndef __FDKGAME_FINDPATH_FINDPATHBASE_H_INCLUDE__
#define __FDKGAME_FINDPATH_FINDPATHBASE_H_INCLUDE__
#include "Base.h"
#include <vector>

namespace fdk { namespace game 
{
	const int FINDPATH_INVALID_NODEID = -1;

	class FDKGAME_API FindPathEnv
	{
	public:		
		struct SuccessorNodeInfo
		{
			int nodeID;
			int cost;
		};
		virtual ~FindPathEnv() {}		
		virtual bool isValidNodeId(int nodeId) const = 0;
		virtual int getNodeCount() const = 0;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const = 0;
		virtual void getSuccessorNodes(int nodeId, std::vector<SuccessorNodeInfo>& result) const = 0;
	};

	class FDKGAME_API FindPathAlgo
	{
	public:
		virtual ~FindPathAlgo() {}
		virtual bool findPath(const FindPathEnv& env, int startNodeID, int targetNodeID) = 0;
	};
}}

#endif
