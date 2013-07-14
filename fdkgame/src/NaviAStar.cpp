#include <fdkgame/NaviAStar.h>

namespace fdk { namespace game { namespace navi
{
	AStar::AStar(const Environment& env, int startNodeID, int targetNodeID, AStarCompleteCondition* completeCondition)
		: m_env(env)
		, m_startNodeID(startNodeID)
		, m_targetNodeID(targetNodeID)
		, m_completeCondition(completeCondition)
		, m_nodeStates(0)
		, m_nodeDatas(0)
		, m_openList()
		, m_bInitedInspect(false)
		, m_searchResult(SearchResult_Proceeding)
		, m_currentClosed()
		, m_closedWithMinHValue()
		, m_minHValue(INT_MAX)		
	{
		FDK_ASSERT(m_env.isValidNodeID(startNodeID));
		FDK_ASSERT(m_env.isValidNodeID(targetNodeID));
		FDK_ASSERT(startNodeID != targetNodeID);

		const size_t NODE_SPACE_SIZE = 512*512;
		static NodeState* s_nodeStates = new NodeState[NODE_SPACE_SIZE];
		static NodeData* s_nodeDatas = new NodeData[NODE_SPACE_SIZE];

		m_nodeStates = s_nodeStates;
		m_nodeDatas = s_nodeDatas;
		memset(m_nodeStates, 0, m_env.getNodeSpaceSize());
	}

	AStar::~AStar()
	{
	}	

	AStar::SearchResult AStar::search(int step, AStarRecorder* recorder)
	{
		FDK_ASSERT(m_searchResult == SearchResult_Proceeding);

		if (!m_bInitedInspect)
		{
			inspectNode(m_startNodeID, INVALID_NODEID, 0, recorder);
			m_bInitedInspect = true;
		}
		
		int proceededStep = 0;
		std::vector<SuccessorNodeInfo> successors;
		while (!m_openList.empty())
		{
			OpenListItem current = m_openList.front();			
			m_openList.pop_front();
			if (m_nodeStates[current.nodeID] != NodeState_Open)
			{
				FDK_ASSERT(m_nodeStates[current.nodeID] == NodeState_Closed);
				continue;
			}

			m_nodeStates[current.nodeID] = NodeState_Closed;
			m_currentClosed = current;
			if (recorder)
			{
				recorder->onCloseNode(m_env, current.nodeID);
			}

			if (m_nodeDatas[current.nodeID].hValue < m_minHValue)
			{
				m_minHValue = m_nodeDatas[current.nodeID].hValue;
				m_closedWithMinHValue = current;
			}

			bool bComplete = false;
			if (m_completeCondition)
			{
				bComplete = m_completeCondition->checkCondition(m_env, m_startNodeID, m_targetNodeID, current.nodeID);
			}
			else
			{
				bComplete = (current.nodeID == m_targetNodeID);
			}
			if (bComplete)
			{
				m_searchResult = SearchResult_Completed;
				return SearchResult_Completed;
			}			

			successors.clear();
			getSuccessorNodes(m_env, current.nodeID, m_nodeDatas[current.nodeID].parentNodeID, successors);
			for (size_t i = 0; i < successors.size(); ++i)
			{
				SuccessorNodeInfo& successor = successors[i];
				inspectNode(successor.nodeID, current.nodeID,
					m_nodeDatas[current.nodeID].gValue+successor.cost, recorder);
			}

			++proceededStep;
			if (step >=1 && proceededStep >= step)
			{
				return SearchResult_Proceeding;
			}
		}

		m_searchResult = SearchResult_PathUnexist;
		return SearchResult_PathUnexist;
	}

	void AStar::inspectNode(int nodeID, int parentNodeID, int gValue, AStarRecorder* recorder)
	{
		switch (m_nodeStates[nodeID])
		{
		case NodeState_Unknown:
			{
				m_nodeStates[nodeID] = NodeState_Open;
				m_nodeDatas[nodeID].parentNodeID = parentNodeID;
				m_nodeDatas[nodeID].gValue = gValue;
				m_nodeDatas[nodeID].hValue = m_env.getHeuristic(nodeID, m_targetNodeID);			
				OpenListItem openListItem(nodeID, m_nodeDatas[nodeID].fValue() );
				m_openList.insert(openListItem);
				if (recorder)
				{
					recorder->onOpenNode(m_env, nodeID, parentNodeID, false);
				}
			}			
			break;
		case NodeState_Open:
			if (gValue < m_nodeDatas[nodeID].gValue)
			{
				m_nodeDatas[nodeID].parentNodeID = parentNodeID;
				m_nodeDatas[nodeID].gValue = gValue;
				OpenListItem openListItem(nodeID, m_nodeDatas[nodeID].fValue() );
				m_openList.insert(openListItem); // 不需要删除旧项，新项必将被先从Open变Close，而旧项由于Close将被跳过
				if (recorder)
				{
					recorder->onOpenNode(m_env, nodeID, parentNodeID, true);
				}
			}
			break;
		case NodeState_Closed:
			break;
		default:
			FDK_ASSERT(0);
		}
	}
	
	void AStar::getPath(std::list<int>& output, int pathOptions) const
	{
		const int endNodeID = (m_searchResult == SearchResult_PathUnexist) ? m_closedWithMinHValue.nodeID : m_currentClosed.nodeID;

		if (endNodeID != m_startNodeID)
		{
			int nodeID = m_nodeDatas[endNodeID].parentNodeID;
			while (nodeID != m_startNodeID)
			{
				output.push_front(nodeID);
				nodeID = m_nodeDatas[nodeID].parentNodeID;
			}
		}
		if (pathOptions & PathOption_WithStartTarget)
		{
			output.push_front(m_startNodeID);
			if (endNodeID != m_startNodeID)
			{
				output.push_back(endNodeID);
			}				
		}
	}
	
	void AStar::getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result)
	{
		env.getSuccessorNodes(*this, nodeID, result);
	}
}}}
