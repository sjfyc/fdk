#ifndef __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#define __FDKGAME_NAVIGRIDBASEDENVUTIL_H_INCLUDE__
#include "NaviBase.h"
#include <list>

namespace fdk { namespace game { namespace navi
{
	bool isDirectlyReachable(const GridEnv& env, int startNodeID, int targetNodeID);
	int getFirstDirectlyReachableNode(const GridEnv& env, int startNodeID, int targetNodeID);
	int getFirstReachableNode(const GridEnv& env, int startNodeID, int targetNodeID);
	int getFirstSameColorReachableNode(const GridEnv& env, int startNodeID, int targetNodeID);

	class GridEnvOctPathPop
	{
	public:
		typedef GridEnv::NodeCoord NodeCoord;
		typedef NodeCoord Direction;
		virtual int pop() = 0;
		virtual bool empty() const = 0;
	protected:
		GridEnvOctPathPop(const GridEnv& env, const std::list<int>& path, bool bCopy);
		virtual ~GridEnvOctPathPop();
		Direction getDirection(const NodeCoord& startNodeCoord, const NodeCoord& targetNodeCoord);
		const GridEnv& m_env;
		const std::list<int>& m_path;
		bool m_bCopy;
	};

	class GridEnvOctPathPopEachNode
		: public GridEnvOctPathPop
	{
		typedef GridEnvOctPathPop _Base;
	public:
		GridEnvOctPathPopEachNode(const GridEnv& env, const std::list<int>& path, bool bCopy=true);
		virtual ~GridEnvOctPathPopEachNode();		
		virtual int pop();
		virtual bool empty() const;
	private:
		std::list<int>::const_iterator m_it;
		NodeCoord m_currentNodeCoord;
		NodeCoord m_nextPortNodeCoord;
		Direction m_nextDirection;
	};

	class GridEnvOctPathPopDirectlyReachableNode
		: public GridEnvOctPathPop
	{
		typedef GridEnvOctPathPop _Base;
	public:
		GridEnvOctPathPopDirectlyReachableNode(const GridEnv& env, const std::list<int>& path, bool bCopy=true);
		virtual ~GridEnvOctPathPopDirectlyReachableNode();		
		virtual int pop();
		virtual bool empty() const;
	private:
		std::list<int>::const_iterator m_it;
		int m_currentNodeID;
	};
}}}

#endif
