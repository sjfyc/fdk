#ifndef __FDKGAME_FINDPATH_FINDPATHASTAR_H_INCLUDE__
#define __FDKGAME_FINDPATH_FINDPATHASTAR_H_INCLUDE__
#include "FindPathBase.h"
#include <queue>

namespace fdk { namespace game 
{
	class FDKGAME_API FindPathAStar
		: public FindPathAlgo
	{
	public:
		FindPathAStar();
		virtual ~FindPathAStar();
		virtual bool findPath(const FindPathEnv& env, int startNodeID, int targetNodeID);
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
		void clear();
		void inspectNode(const FindPathEnv& env, int testNodeID, int parentNodeID, int targetNodeID, int gValue);
		NodeState* m_nodeStates;
		NodeData* m_nodeDatas;
		OpenList* m_openList;
	};

	inline int FindPathAStar::NodeData::fValue() const
	{
		return gValue + hValue; 
	}

	inline bool FindPathAStar::OpenListItem::operator<(const OpenListItem& other) const
	{
		return fValue > other.fValue;
	}
}}

#endif
