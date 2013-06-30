#ifndef __FDKGAME_NAVIBASE_H_INCLUDE__
#define __FDKGAME_NAVIBASE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include "Math.h"

namespace fdk { namespace game { namespace navi
{
	class Environment;
	class GridBasedEnv;
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
		virtual const GridBasedEnv* toGridBaseEnv() const;
		virtual GridBasedEnv* toGridBaseEnv();
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
		virtual const GridBasedEnv* toGridBaseEnv() const;
		virtual GridBasedEnv* toGridBaseEnv();
	};

	class FDKGAME_API Navigator
	{
	protected:
		Navigator();
		virtual ~Navigator();
	};
	
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
	{
	}

	inline Navigator::~Navigator()
	{
	}
}}}

#endif
