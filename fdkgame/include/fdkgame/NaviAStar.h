#ifndef __FDKGAME_NAVIASTAR_H_INCLUDE__
#define __FDKGAME_NAVIASTAR_H_INCLUDE__
#include "NaviBase.h"
#include <list>
#include <fdk/BinaryHeap.h>

namespace fdk { namespace game { namespace navi
{
	class FDKGAME_API AStarRecorder
	{
	public:
		virtual ~AStarRecorder() {}
		virtual void onOpenNode(const Environment& env, int nodeID, int parentNodeID, bool bReopen) = 0;
		virtual void onCloseNode(const Environment& env, int nodeID) = 0;
	};

	class FDKGAME_API AStarCompleteCondition
	{
	public:
		virtual bool checkCondition(const Environment& env, int startNodeID, int targetNodeID, int nodeID) const = 0;
	};

	class FDKGAME_API AStar
		: public Navigator
	{
	public:
		enum SearchResult
		{			
			SearchResult_Proceeding,
			SearchResult_Completed,
			SearchResult_PathUnexist,
		};
		enum PathOption
		{
			PathOption_WithStart=(1<<0),
			PathOption_WithTarget=(1<<1),			
			PathOption_WithTargetIfEmpty=(1<<2),
			PathOption_WithStartTarget=(PathOption_WithStart|PathOption_WithTarget),
		};
		AStar(const Environment& env, int startNodeID, int targetNodeID, AStarCompleteCondition* completeCondition=0);
		virtual ~AStar();
		SearchResult search(int step=-1, AStarRecorder* recorder=0);
		SearchResult getSearchResult() const;
		void getPath(std::list<int>& output, int pathOptions=0) const;
		int getPathCost() const;		
		const Environment& getEnvironment() const;
		int getStartNodeID() const;
		int getTargetNodeID() const;
		int getCompletedNodeID() const;
		int getClosestNodeID() const;
		virtual void getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result);
	private:
		enum NodeStateEnum
		{
			NodeState_Unknown = 0,
			NodeState_Open,
			NodeState_Closed,
		};
		typedef unsigned char NodeState;
		struct NodeData
		{
			int parentNodeID;
			int gValue;
			int hValue;
			int fValue() const;
		};
		struct OpenListItem
		{			
			int nodeID;
			int fValue;
			OpenListItem();
			OpenListItem(int nodeID, int fValue);
			bool operator<(const OpenListItem& other) const;
		};
		typedef fdk::BinaryHeap<OpenListItem> OpenList;
		void inspectNode(int nodeID, int parentNodeID, int gValue, AStarRecorder* recorder);
		const Environment& m_env;
		int m_startNodeID;
		int m_targetNodeID;
		AStarCompleteCondition* m_completeCondition;
		NodeState* m_nodeStates;
		NodeData* m_nodeDatas;
		OpenList m_openList;
		bool m_bInitedInspect;
		SearchResult m_searchResult;
		OpenListItem m_currentClosed;
		OpenListItem m_closest;
		int m_minHValue;		
	};

	inline int AStar::NodeData::fValue() const
	{
		return gValue + hValue; 
	}

	inline bool AStar::OpenListItem::operator<(const OpenListItem& other) const
	{
		return fValue < other.fValue;
	}

	inline AStar::OpenListItem::OpenListItem()
		: nodeID(INVALID_NODEID)
		, fValue(0)
	{
	}
	
	inline AStar::OpenListItem::OpenListItem(int l_nodeID, int l_fValue)
		: nodeID(l_nodeID)
		, fValue(l_fValue)
	{
	}

	inline AStar::SearchResult AStar::getSearchResult() const
	{
		return m_searchResult;
	}

	inline int AStar::getPathCost() const
	{
		return (m_searchResult == SearchResult_PathUnexist) ? m_closest.fValue : m_currentClosed.fValue;
	}

	inline const Environment& AStar::getEnvironment() const
	{
		return m_env;
	}	

	inline int AStar::getStartNodeID() const
	{
		return m_startNodeID;
	}

	inline int AStar::getTargetNodeID() const	
	{
		return m_targetNodeID;
	}

	inline int AStar::getCompletedNodeID() const
	{
		return (m_searchResult == SearchResult_Completed) ? m_currentClosed.nodeID : INVALID_NODEID;
	}	

	inline int AStar::getClosestNodeID() const
	{
		return m_closest.nodeID;
	}
}}}

#endif
