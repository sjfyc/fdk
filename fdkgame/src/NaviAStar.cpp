#include <fdkgame/NaviAStar.h>

namespace fdk { namespace game { namespace navi
{
	AStar::AStar(const Environment& env, int startNodeID, int targetNodeID)
		: m_env(env)
		, m_startNodeID(startNodeID)
		, m_targetNodeID(targetNodeID)
		, m_nodeStates(0)
		, m_nodeDatas(0)
		, m_openList()
		, m_searchResult(SearchResult_Proceeding)
		, m_path()
		, m_pathCost(PATHUNEXIST_COST)
		, m_recorder(0)
		, m_bInitedInspect(false)
	{
		FDK_ASSERT(m_env.isValidNodeID(startNodeID));
		FDK_ASSERT(m_env.isValidNodeID(targetNodeID));
		FDK_ASSERT(startNodeID != targetNodeID);

		const size_t NODE_SPACE_SIZE = 2048*2048;
		static NodeState* s_nodeStates = new NodeState[NODE_SPACE_SIZE];
		static NodeData* s_nodeDatas = new NodeData[NODE_SPACE_SIZE];

		m_nodeStates = s_nodeStates;
		m_nodeDatas = s_nodeDatas;
		memset(m_nodeStates, 0, m_env.getNodeSpaceSize());
	}

	AStar::~AStar()
	{
		//FDK_DELETE_ARRAY(m_nodeStates);
		//FDK_DELETE_ARRAY(m_nodeDatas);
	}	

	AStar::SearchResult AStar::search(int step)
	{
		if (!m_bInitedInspect)
		{
			inspectNode(m_startNodeID, INVALID_NODEID, 0);
			m_bInitedInspect = true;
		}
		if (m_searchResult != SearchResult_Proceeding)
		{
			return m_searchResult;
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
			if (m_recorder)
			{
				m_recorder->onCloseNode(m_env, current.nodeID);
			}

			if (current.nodeID == m_targetNodeID)
			{
				buildPath();
				m_pathCost = current.fValue;
				m_searchResult = SearchResult_Completed;
				return SearchResult_Completed;
			}			

			successors.clear();
			getSuccessorNodes(m_env, current.nodeID, m_nodeDatas[current.nodeID].parentNodeID, successors);
			for (size_t i = 0; i < successors.size(); ++i)
			{
				SuccessorNodeInfo& successor = successors[i];
				inspectNode(successor.nodeID, current.nodeID,
					m_nodeDatas[current.nodeID].gValue+successor.cost);
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

	void AStar::inspectNode(int nodeID, int parentNodeID, int gValue)
	{
		switch (m_nodeStates[nodeID])
		{
		case NodeState_Unknown:
			{
				m_nodeStates[nodeID] = NodeState_Open;
				m_nodeDatas[nodeID].parentNodeID = parentNodeID;
				m_nodeDatas[nodeID].gValue = gValue;
				m_nodeDatas[nodeID].hValue = m_env.getHeuristic(nodeID, m_targetNodeID);			
				OpenListItem openListItem = { nodeID, m_nodeDatas[nodeID].fValue() };
				m_openList.insert(openListItem);
				if (m_recorder)
				{
					m_recorder->onOpenNode(m_env, nodeID, parentNodeID, false);
				}
			}			
			break;
		case NodeState_Open:
			if (gValue < m_nodeDatas[nodeID].gValue)
			{
				m_nodeDatas[nodeID].parentNodeID = parentNodeID;
				m_nodeDatas[nodeID].gValue = gValue;
				OpenListItem openListItem = { nodeID, m_nodeDatas[nodeID].fValue() };				
				m_openList.insert(openListItem); // 不需要删除旧项，新项必将被先从Open变Close，而旧项由于Close将被跳过
				if (m_recorder)
				{
					m_recorder->onOpenNode(m_env, nodeID, parentNodeID, true);
				}
			}
			break;
		case NodeState_Closed:
			break;
		default:
			FDK_ASSERT(0);
		}
	}

	void AStar::buildPath()
	{
		FDK_ASSERT(m_path.empty());
		int nodeId = m_targetNodeID;
		while (1)
		{
			m_path.push_back(nodeId);
			const int parentNodeID = m_nodeDatas[nodeId].parentNodeID;
			if (parentNodeID == INVALID_NODEID)
			{
				break;
			}
			nodeId = parentNodeID;
		}
	}

	void AStar::getSuccessorNodes(const Environment& env, int nodeID, int parentNodeID, std::vector<SuccessorNodeInfo>& result)
	{
		env.getSuccessorNodes(*this, nodeID, result);
	}

}}}
