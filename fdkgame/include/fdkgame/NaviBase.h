#ifndef __FDKGAME_NAVIBASE_H_INCLUDE__
#define __FDKGAME_NAVIBASE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include "Math.h"

namespace fdk { namespace game { namespace navi
{
	class Environment;
	class GridEnv;
	class Navigator;

	const int INVALID_NODEID = -1;	
	const int COST_STRAIGHT = 100;
	const int COST_DIAGONAL = 142;
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
		virtual const GridEnv* toGridEnv() const;
		virtual GridEnv* toGridEnv();
	};

	class FDKGAME_API Navigator
	{
	protected:
		Navigator();
		virtual ~Navigator();
	};

	class FDKGAME_API GridEnv
		: public Environment
	{
	public:
		typedef Vector2D<int> NodeCoord;
		virtual ~GridEnv() {}
		virtual int getSizeX() const = 0;
		virtual int getSizeY() const = 0;
		virtual bool isValidNodeCoord(const NodeCoord& nodeCoord) const;
		virtual bool isNodeReachable(int nodeID) const = 0;

		int toNodeID(const NodeCoord& nodeCoord) const;
		NodeCoord toNodeCoord(int nodeID) const;
		bool isNodeWithCoordReachable(const NodeCoord& nodeCoord) const;

		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual const GridEnv* toGridEnv() const;
		virtual GridEnv* toGridEnv();
		void getSuccessorNodesWithoutCheck(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
	protected:
		int getHeuristicManhattan(int startNodeID, int targetNodeID) const;
		int getHeuristicChebyshev(int startNodeID, int targetNodeID) const;
		int getHeuristicEuclidean(int startNodeID, int targetNodeID) const;
		void getSuccessorNodes(Navigator& navigator, int nodeID, std::vector<SuccessorNodeInfo>& result, bool bCutCorner) const;
	private:
		bool tryAddSuccessorNode(Navigator& navigator, std::vector<SuccessorNodeInfo>& result, const NodeCoord& nodeCoord, int cost, int parentNodeID) const;
		bool tryAddSuccessorNodeWithoutCheck(std::vector<SuccessorNodeInfo>& result, const NodeCoord& nodeCoord, int cost, int parentNodeID) const;
	};
	
	class FDKGAME_API GridPartEnv
		: public GridEnv
	{		
	public:
		typedef Rect<NodeCoord::ValueType> Range;
		GridPartEnv(const GridEnv& outer, const Range& range);
		const GridEnv& getOuter() const;
		const Range& getRange() const;
		// GridEnv interfaces
		virtual int getSizeX() const;
		virtual int getSizeY() const;
		virtual bool isValidNodeCoord(const NodeCoord& nodeCoord) const;
		virtual bool isNodeReachable(int nodeID) const;		
	private:
		const GridEnv& m_outer;
		Range m_range;
	};
		
	inline bool Environment::isValidNodeID(int nodeID) const
	{
		return nodeID >= 0 && nodeID < getNodeSpaceSize();
	}

	inline Navigator::Navigator()
	{
	}

	inline Navigator::~Navigator()
	{
	}

	inline int GridEnv::toNodeID(const NodeCoord& nodeCoord) const
	{
		FDK_ASSERT(isValidNodeCoord(nodeCoord));
		return nodeCoord.y * getSizeX() + nodeCoord.x;
	}

	inline GridEnv::NodeCoord GridEnv::toNodeCoord(int nodeID) const
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		return NodeCoord(nodeID % getSizeX(), nodeID / getSizeX());
	}

	inline bool GridEnv::isNodeWithCoordReachable(const NodeCoord& nodeCoord) const
	{
		return isNodeReachable(toNodeID(nodeCoord));
	}

	inline GridPartEnv::GridPartEnv(const GridEnv& outer, const Range& range)
		: m_outer(outer)
		, m_range(range)
	{
	}

	inline const GridEnv& GridPartEnv::getOuter() const
	{
		return m_outer;
	}

	inline const GridPartEnv::Range& GridPartEnv::getRange() const
	{
		return m_range;
	}
}}}

#endif
