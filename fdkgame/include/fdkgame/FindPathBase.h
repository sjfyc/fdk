#ifndef __FDKGAME_FINDPATHBASE_H_INCLUDE__
#define __FDKGAME_FINDPATHBASE_H_INCLUDE__
#include "Base.h"
#include <vector>

namespace fdk { namespace game { namespace findpath
{
	const int INVALID_NODEID = -1;

	struct SuccessorNodeInfo
	{
		int nodeID;
		int cost;
	};

	// 环境要求：所有节点ID必须从0开始并依次自增
	class FDKGAME_API Environment
	{
	public:
		virtual ~Environment() {}
		virtual bool isValidNodeID(int nodeID) const;
		virtual int getNodeSpaceSize() const = 0;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const = 0;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const = 0;
		virtual bool isObstacle(int nodeID) const = 0;
	};
}}}

#endif
