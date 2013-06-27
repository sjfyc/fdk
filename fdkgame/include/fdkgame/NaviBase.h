#ifndef __FDKGAME_NAVIBASE_H_INCLUDE__
#define __FDKGAME_NAVIBASE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include "Math.h"

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
		virtual bool isNodeReachable(int nodeID) const = 0;
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
		bool isNodeWithCoordReachable(const NodeCoord& nodeCoord) const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const;
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

	inline bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	inline int GridBasedEnv::toNodeID(const NodeCoord& nodeCoord) const
	{
		FDK_ASSERT(isValidNodeCoord(nodeCoord));
		return nodeCoord.y * getSizeX() + nodeCoord.x;
	}

	inline GridBasedEnv::NodeCoord GridBasedEnv::toNodeCoord(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return NodeCoord(nodeID % getSizeX(), nodeID / getSizeX());
	}

	inline int GridBasedEnv::getNodeSpaceSize() const
	{
		return getSizeX() * getSizeY();
	}

	inline bool GridBasedEnv::isValidNodeCoord(const NodeCoord& nodeCoord) const
	{
		return nodeCoord.x >= 0 && nodeCoord.x < getSizeX()
			&& nodeCoord.y >= 0 && nodeCoord.y < getSizeY();
	}

	inline bool GridBasedEnv::isNodeWithCoordReachable(const NodeCoord& nodeCoord) const
	{
		return isNodeReachable(toNodeID(nodeCoord));
	}

	inline Navigator::Navigator()
		: m_environmentChecker(0)
	{
	}

	inline Navigator::~Navigator()
	{
	}
}}}

#endif
