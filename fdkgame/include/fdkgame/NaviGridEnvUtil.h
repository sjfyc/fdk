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
		typedef GridEnv::NodeCoord NodeCoord;
		typedef NodeCoord Direction;
		virtual int pop() = 0;
	protected:
		GridEnvOctPathPop(const GridEnv& env, const std::vector<int>& path, bool bCopy);
		virtual ~GridEnvOctPathPop();
		Direction getDirection(const NodeCoord& startNodeCoord, const NodeCoord& targetNodeCoord);
		const GridEnv& m_env;
		const std::vector<int>& m_path;
		bool m_bCopy;
	};

	class GridEnvOctPathPopEachNode
		: public GridEnvOctPathPop
	{
		typedef GridEnvOctPathPop _Base;
	public:
		GridEnvOctPathPopEachNode(const GridEnv& env, const std::vector<int>& path, bool bCopy=true);
		~GridEnvOctPathPopEachNode();
		virtual int pop();
	private:
		int m_index;
		NodeCoord m_currentNodeCoord;
		NodeCoord m_nextPortNodeCoord;
		Direction m_nextDirection;
	};
}}}

#endif
