#include <fdkgame/FindPathAStar.h>

namespace fdk { namespace game { namespace findpath
{
	AStar::AStar(const Environment& env, int startNodeID, int targetNodeID)
		: m_env(env)
		, m_startNodeID(startNodeID)
		, m_targetNodeID(targetNodeID)
		, m_nodeStates(0)
		, m_nodeDatas(0)
		, m_openList()
	{
		FDK_ASSERT(startNodeID != targetNodeID);
		FDK_ASSERT(m_env.isValidNodeID(startNodeID));
		FDK_ASSERT(m_env.isValidNodeID(targetNodeID));

		const int nodeCount = m_env.getNodeCount();
		m_nodeStates = new NodeState[nodeCount];
		m_nodeDatas = new NodeData[nodeCount];
		memset(m_nodeStates, 0, sizeof(NodeState)*nodeCount);

		inspectNode(m_startNodeID, INVALID_NODEID, 0);
	}

	AStar::~AStar()
	{
		FDK_DELETE_ARRAY(m_nodeStates);
		FDK_DELETE_ARRAY(m_nodeDatas);
	}
	
	AStar::SearchResult AStar::search(int step)
	{
		int proceededStep = 0;
		std::vector<SuccessorNodeInfo> successors;
		while (!m_openList.empty())
		{
			if (step >=1 && proceededStep >= step)
			{
				return SearchResult_OK;
			}
			++proceededStep;

			OpenListItem current = m_openList.top();			
			m_openList.pop();
			m_nodeStates[current.nodeID] = NodeState_Closed;			

			if (current.nodeID == m_targetNodeID)
			{
				buildPath();
				return SearchResult_Completed;
			}			

			successors.clear();
			m_env.getSuccessorNodes(current.nodeID, successors);
			for (size_t i = 0; i < successors.size(); ++i)
			{
				SuccessorNodeInfo& successor = successors[i];
				inspectNode(successor.nodeID, current.nodeID,
					m_nodeDatas[current.nodeID].gValue+successor.cost);
			}
		}
		return SearchResult_NoPath;
	}

	void AStar::inspectNode(int testNodeID, int parentNodeID, int gValue)
	{
		switch (m_nodeStates[testNodeID])
		{
		case NodeState_Unknown:
			{
				m_nodeStates[testNodeID] = NodeState_Open;
				m_nodeDatas[testNodeID].parentNodeID = parentNodeID;
				m_nodeDatas[testNodeID].gValue = gValue;
				m_nodeDatas[testNodeID].hValue = m_env.getHeuristic(testNodeID, m_targetNodeID);			
				OpenListItem openListItem = { testNodeID, m_nodeDatas[testNodeID].fValue() };
				m_openList.push(openListItem);
			}			
			break;
		case NodeState_Open:
			if (gValue < m_nodeDatas[testNodeID].gValue)
			{
				m_nodeDatas[testNodeID].parentNodeID = parentNodeID;
				m_nodeDatas[testNodeID].gValue = gValue;
				OpenListItem openListItem = { testNodeID, m_nodeDatas[testNodeID].fValue() };				
				m_openList.push(openListItem); // 不需要删除旧项，新项必将被先从Open变Close，而旧项由于Close将被跳过
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
			if (nodeId == m_startNodeID)
				break;
			m_path.push_back(nodeId);			
			if (m_env.isObstacle(nodeId)) // 障碍也可以以一个很大的代价称为后继节点
			{
				m_path.clear();
			}
			nodeId = m_nodeDatas[nodeId].parentNodeID;
		}
	}

}}}
