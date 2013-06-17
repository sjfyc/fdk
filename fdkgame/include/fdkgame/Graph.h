#ifndef __FDKGAME_GRAPH_H_INCLUDE__
#define __FDKGAME_GRAPH_H_INCLUDE__
#include "Grap.hpp"

namespace fdk { namespace game
{
	template <class NodeInfoT, class EdgeInfoT>
	inline bool Graph<NodeInfoT, EdgeInfoT>::CmpNode::operator()(const NodeType* a, const NodeType* b) const
	{
		return a->getID() < b->getID();
	}
	
	template <class NodeInfoT, class EdgeInfoT>
	inline Graph<NodeInfoT, EdgeInfoT>::Graph()
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline Graph<NodeInfoT, EdgeInfoT>::~Graph()
	{
		clear();
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline void Graph<NodeInfoT, EdgeInfoT>::clear()
	{
		for (Nodes::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			delete *it;
		}
		m_nodes.clear();
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::addNode(int nodeID, const NodeInfoT& info)
	{
		FDK_ASSERT(!findNode(nodeID));
		NodeType* node = new NodeType(nodeID, info);
		bool result = m_nodes.insert(node).second;
		FDK_ASSERT(result);
		return *node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline void Graph<NodeInfoT, EdgeInfoT>::removeNode(NodeType& node)
	{
		FDK_ASSERT(findNode(node.getID()) == &node);
		bool result = m_nodes.erase(&node) > 0;
		FDK_ASSERT(result);
		delete &node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const typename Graph<NodeInfoT, EdgeInfoT>::Nodes& Graph<NodeInfoT, EdgeInfoT>::getNodes() const
	{
		return m_nodes;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>* Graph<NodeInfoT, EdgeInfoT>::findNode(int nodeID)
	{
		return const_cast<GraphNode<NodeInfoT, EdgeInfoT>* >(
			static_cast<const _Self&>(*this).findNode(nodeID)
			);
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphNode<NodeInfoT, EdgeInfoT>* Graph<NodeInfoT, EdgeInfoT>::findNode(int nodeID) const
	{
		FDK_CMP_PTR(NodeType, search);
		search->m_id = nodeID;
		Nodes::const_iterator it = m_nodes.find(search);
		if (it == m_nodes.end())
		{
			return NULL;
		}
		return *it;
	}

	template <class NodeInfoT, class EdgeInfoT>
	GraphEdge<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::addOutEdge(NodeType& startNode, const NodeType& targetNode, const EdgeInfoT& info)
	{
		FDK_ASSERT(findNode(startNode.getID()) == &startNode);
		FDK_ASSERT(findNode(targetNode.getID()) == &targetNode);
		return startNode.addOutEdge(targetNode.getID(), info);
	}
}

#endif
