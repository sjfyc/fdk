#include <fdkgame/FindPathAStar.h>

namespace fdk { namespace game 
{
	FindPathAStar::FindPathAStar()
		: m_nodeStates(0)
		, m_nodeDatas(0)
		, m_openList(0)
	{
	}

	FindPathAStar::~FindPathAStar()
	{
		clear();
	}

	void FindPathAStar::clear()
	{
		FDK_DELETE_ARRAY(m_nodeStates);
		FDK_DELETE_ARRAY(m_nodeDatas);
		FDK_DELETE(m_openList);
	}

	bool FindPathAStar::findPath(const FindPathEnv& env, int startNodeID, int targetNodeID)
	{
		FDK_ASSERT(env.isValidNodeId(startNodeID));
		FDK_ASSERT(env.isValidNodeId(targetNodeID));		

		// prepare for new find
		clear();
		const int nodeCount = env.getNodeCount();
		m_nodeStates = new NodeState[nodeCount];
		m_nodeDatas = new NodeData[nodeCount];
		m_openList = new OpenList;
		memset(m_nodeStates, 0, sizeof(NodeState)*nodeCount);

		inspectNode(env, startNodeID, FINDPATH_INVALID_NODEID, targetNodeID, 0);

		// main loop
		std::vector<FindPathEnv::SuccessorNodeInfo> successors;
		while (!m_openList->empty())
		{
			OpenListItem current = m_openList->top();			
			if (current.nodeID == targetNodeID)
			{
				// todo finish;
				return true;
			}
			m_openList->pop();
			m_nodeStates[current.nodeID] = NodeState_Closed;

			successors.clear();
			env.getSuccessorNodes(current.nodeID, successors);
			for (size_t i = 0; i < successors.size(); ++i)
			{
				FindPathEnv::SuccessorNodeInfo& successor = successors[i];
				inspectNode(env, successor.nodeID, current.nodeID, targetNodeID,
					m_nodeDatas[current.nodeID].gValue+successor.cost);
			}
		}
		return true;
	}

	//void FindPathAStar::findPathMain(int startNodeID)
	//{
	//	int maxopen = 0;
	//	//    int closedsize = 0;
	//	int numberNodes = m_env->getNumberNodes();
	//	m_closed->init(numberNodes);
	//	m_open.init(numberNodes);
	//	int heuristic = m_env->getHeuristic(start, m_target);
	//	m_pathCost = NO_COST;
	//	AStarNode startNode(start, NO_NODE, 0, heuristic);
	//	m_open.insert(startNode);
	//	vector<Environment::Successor> successors;
	//	while (! m_open.isEmpty())
	//	{
	//		m_statistics.get("open_length").add(m_open.getSize());
	//		if (m_open.getSize() > maxopen)
	//			maxopen = m_open.getSize();
	//		//m_open.print(cout);
	//		AStarNode node = getBestNodeFromOpen();
	//		//cout << '[';  node.print(cout); cout << ']' << endl;
	//		if (node.m_nodeId == m_target)
	//		{
	//			finishSearch(start, node);
	//			return;
	//		}
	//		++m_nodesExpanded;
	//		m_env->getSuccessors(node.m_nodeId, NO_NODE, successors);
	//		m_branchingFactor.add(successors.size());
	//		for (vector<Environment::Successor>::const_iterator i
	//			= successors.begin(); i != successors.end(); ++i)
	//		{
	//			int newg = node.m_g + i->m_cost;
	//			int target = i->m_target;
	//			const AStarNode* targetAStarNode = findNode(target);
	//			if (targetAStarNode != 0)
	//			{
	//				if (newg >= targetAStarNode->m_g)
	//					continue;
	//				if (! m_open.remove(target))
	//					m_closed->remove(target);
	//			}
	//			int newHeuristic = m_env->getHeuristic(target, m_target);
	//			AStarNode newAStarNode(target, node.m_nodeId, newg, newHeuristic);
	//			m_open.insert(newAStarNode);
	//		}
	//		//        closedsize++;
	//		m_closed->add(node);
	//		//        m_statistics.get("closed_length").add(m_closed.size());
	//		//closed->print(cout);
	//	}
	//	m_statistics.get("open_max").add(maxopen);
	//}

	void FindPathAStar::inspectNode(const FindPathEnv& env, int testNodeID, int parentNodeID, int targetNodeID, int gValue)
	{
		switch (m_nodeStates[testNodeID])
		{
		case NodeState_Unknown:
			{
				m_nodeStates[testNodeID] = NodeState_Open;
				m_nodeDatas[testNodeID].parentNodeID = parentNodeID;
				m_nodeDatas[testNodeID].gValue = gValue;
				m_nodeDatas[testNodeID].hValue = env.getHeuristic(testNodeID, targetNodeID);			
				OpenListItem openListItem = { testNodeID, m_nodeDatas[testNodeID].fValue() };
				m_openList->push(openListItem);
			}			
			break;
		case NodeState_Open:
			if (gValue < m_nodeDatas[testNodeID].gValue)
			{
				m_nodeDatas[testNodeID].parentNodeID = parentNodeID;
				m_nodeDatas[testNodeID].gValue = gValue;
				OpenListItem openListItem = { testNodeID, m_nodeDatas[testNodeID].fValue() };				
				m_openList->push(openListItem); // 不需要删除旧项，新项必将被先从Open变Close，而旧项由于Close将被跳过
			}
			break;
		case NodeState_Closed:
			break;
		default:
			FDK_ASSERT(0);
		}
	}
}}
