#ifndef __FDKGAME_FINDPATHBASE_H_INCLUDE__
#define __FDKGAME_FINDPATHBASE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include "Math.h"

namespace fdk { namespace game { namespace findpath
{
	class Environment;
	class EnvironmentChecker;
	class PathFinder;
	class GridMap;
	class HpaMap;
	class HpaCluster;

	const int INVALID_NODEID = -1;
	const int PATHUNEXIST_COST = -1;

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
		virtual void getSuccessorNodes(PathFinder& pathFinder, int nodeID, std::vector<SuccessorNodeInfo>& result) const = 0;
		virtual bool isObstacle(int nodeID) const = 0;
	};

	class FDKGAME_API GridBasedEnv
		: public Environment
	{
	public:
		typedef Vector2D<int> NodeCoord;
		virtual ~GridBasedEnv() {}
		virtual int getSizeX() const = 0;
		virtual int getSizeY() const = 0;
		int toNodeID(const NodeCoord& nodeCoord) const;
		NodeCoord toNodeCoord(int nodeID) const;
		bool isValidNodeCoord(const NodeCoord& nodeCoord) const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const;
	};

	class FDKGAME_API EnvironmentChecker
	{
	public:
		virtual void onSearchBegining(const GridMap& env, int startNodeID, int endNodeID) {}
		virtual void onSearchEnded(const GridMap& env, int startNodeID, int endNodeID) {}
		virtual bool checkSuccessorNode(const GridMap& env, int nodeID, int parentNodeID) const { return true; }

		virtual void onSearchBegining(const HpaMap& env, int startNodeID, int endNodeID) {}
		virtual void onSearchEnded(const HpaMap& env, int startNodeID, int endNodeID) {}
		virtual bool checkSuccessorNode(const HpaMap& env, int nodeID, int parentNodeID) const { return true; }

		virtual void onSearchBegining(const HpaCluster& env, int startNodeID, int endNodeID) {}
		virtual void onSearchEnded(const HpaCluster& env, int startNodeID, int endNodeID) {}
		virtual bool checkSuccessorNode(const HpaCluster& env, int nodeID, int parentNodeID) const { return true; }
	};

	class FDKGAME_API PathFinder
	{
	public:
		EnvironmentChecker* setEnvironmentChecker(EnvironmentChecker* checker);
		EnvironmentChecker* getEnvironmentChecker() const;
	protected:
		PathFinder();
		virtual ~PathFinder();
	private:
		EnvironmentChecker* m_environmentChecker;
	};
	
	inline EnvironmentChecker* PathFinder::setEnvironmentChecker(EnvironmentChecker* checker)
	{
		EnvironmentChecker* old = m_environmentChecker;
		m_environmentChecker = checker;
		return old;
	}

	inline EnvironmentChecker* PathFinder::getEnvironmentChecker() const
	{
		return m_environmentChecker;
	}
}}}

#endif
