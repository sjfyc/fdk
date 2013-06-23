#ifndef __FDKGAME_NAVIBASE_H_INCLUDE__
#define __FDKGAME_NAVIBASE_H_INCLUDE__
#include "Base.h"
#include <vector>

namespace fdk { namespace game { namespace navi
{
	class Environment;
	class EnvironmentChecker;
	class Navigator;

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
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const = 0;
	};

	class FDKGAME_API EnvironmentChecker
	{
	public:
		//virtual void onSearchBegining(const GridMap& env, int startNodeID, int endNodeID) {}
		//virtual void onSearchEnded(const GridMap& env, int startNodeID, int endNodeID) {}
		//virtual bool checkSuccessorNode(const GridMap& env, int nodeID, int parentNodeID) const { return true; }

		//virtual void onSearchBegining(const HpaMap& env, int startNodeID, int endNodeID) {}
		//virtual void onSearchEnded(const HpaMap& env, int startNodeID, int endNodeID) {}
		//virtual bool checkSuccessorNode(const HpaMap& env, int nodeID, int parentNodeID) const { return true; }

		//virtual void onSearchBegining(const HpaCluster& env, int startNodeID, int endNodeID) {}
		//virtual void onSearchEnded(const HpaCluster& env, int startNodeID, int endNodeID) {}
		//virtual bool checkSuccessorNode(const HpaCluster& env, int nodeID, int parentNodeID) const { return true; }
	};

	class FDKGAME_API Navigator
	{
	public:
		EnvironmentChecker* setEnvironmentChecker(EnvironmentChecker* checker);
		EnvironmentChecker* getEnvironmentChecker() const;
	protected:
		Navigator();
		virtual ~Navigator();
	private:
		EnvironmentChecker* m_environmentChecker;
	};
	
	inline EnvironmentChecker* Navigator::setEnvironmentChecker(EnvironmentChecker* checker)
	{
		EnvironmentChecker* old = m_environmentChecker;
		m_environmentChecker = checker;
		return old;
	}

	inline EnvironmentChecker* Navigator::getEnvironmentChecker() const
	{
		return m_environmentChecker;
	}
}}}

#endif
