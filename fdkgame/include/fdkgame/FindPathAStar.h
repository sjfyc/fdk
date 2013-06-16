#ifndef __FDKGAME_FINDPATHASTAR_H_INCLUDE__
#define __FDKGAME_FINDPATHASTAR_H_INCLUDE__
#include "FindPathBase.h"
#include <queue>

namespace fdk { namespace game { namespace findpath
{
	class FDKGAME_API AStarRecorder
	{
	public:
		virtual ~AStarRecorder() {}
		virtual void onOpenNode(int nodeID, int parentNodeID, bool bReopen) = 0;
		virtual void onCloseNode(int nodeID) = 0;
	};

	class FDKGAME_API AStar
	{
	public:
		enum SearchResult
		{			
			SearchResult_Proceeding,
			SearchResult_Completed,
			SearchResult_NoPath,
		};
		AStar(const Environment& env, int startNodeID, int targetNodeID, AStarRecorder* recorder=0);
		~AStar();
		SearchResult search(int step=-1);
		const std::vector<int>& getPath() const;
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
			bool operator<(const OpenListItem& other) const;
		};
		typedef std::priority_queue<OpenListItem> OpenList;
		void inspectNode(int nodeID, int parentNodeID, int gValue);
		void buildPath();
		const Environment& m_env;
		int m_startNodeID;
		int m_targetNodeID;
		NodeState* m_nodeStates;
		NodeData* m_nodeDatas;
		OpenList m_openList;
		SearchResult m_searchResult;
		std::vector<int> m_path;
		AStarRecorder* m_recorder;
	};

	inline int AStar::NodeData::fValue() const
	{
		return gValue + hValue; 
	}

	inline bool AStar::OpenListItem::operator<(const OpenListItem& other) const
	{
		return fValue > other.fValue;
	}

	inline const std::vector<int>& AStar::getPath() const
	{
		return m_path;
	}
}}}

#endif
