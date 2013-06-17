#ifndef __FDKGAME_GRAPHNODE_H_INCLUDE__
#define __FDKGAME_GRAPHNODE_H_INCLUDE__
#include "GraphNode.hpp"
#include "GraphEdge.h"

namespace fdk { namespace game 
{	
	template <class NodeInfoT, class EdgeInfoT>
	inline bool GraphNode<NodeInfoT, EdgeInfoT>::CmpEdge::operator()(const EdgeType* a, const EdgeType* b) const
	{
		return a->getTargetNodeID() < b->getTargetNodeID();
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>::GraphNode(int id, const NodeInfoT& info)
		: m_id(id)
		, m_info(info)
		, m_outEdges()
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>::~GraphNode()
	{
		clearOutEdges();
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline int GraphNode<NodeInfoT, EdgeInfoT>::getID() const
	{
		return m_id;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline NodeInfoT& GraphNode<NodeInfoT, EdgeInfoT>::getInfo()
	{
		return m_info;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const NodeInfoT& GraphNode<NodeInfoT, EdgeInfoT>::getInfo() const
	{
		return m_info;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const typename GraphNode<NodeInfoT, EdgeInfoT>::OutEdges& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdges() const
	{
		return m_outEdges;
	}

	template <class NodeInfoT, class EdgeInfoT>
	GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findOutEdge(int targetNodeID)
	{
		return const_cast<GraphEdge<NodeInfoT, EdgeInfoT>* >(
			static_cast<const _Self&>(*this).findOutEdge(targetNodeID)
			);
	}

	template <class NodeInfoT, class EdgeInfoT>
	const GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findOutEdge(int targetNodeID) const
	{
		FDK_CMP_PTR(EdgeType, search);
		search->m_targetNodeID = targetNodeID;
		OutEdges::const_iterator it = m_outEdges.find(search);
		if (it == m_outEdges.end())
		{
			return NULL;
		}
		return *it;
	}
	
	template <class NodeInfoT, class EdgeInfoT>
	void GraphNode<NodeInfoT, EdgeInfoT>::clearOutEdges()
	{
		for (OutEdges::iterator it = m_outEdges.begin(); it != m_outEdges.end(); ++it)
		{
			delete *it;
		}
		m_outEdges.clear();
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::addOutEdge(int targetNodeID, const EdgeInfoT& info)
	{
		FDK_ASSERT(!findOutEdge(targetNodeID));
		EdgeType* edge = new EdgeType(targetNodeID, info);
		bool result = m_outEdges.insert(edge).second;
		FDK_ASSERT(result);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	void GraphNode<NodeInfoT, EdgeInfoT>::removeOutEdge(EdgeType& edge)
	{
		FDK_ASSERT(findOutEdge(edge.getTargetNodeID()) == &edge);
		bool result = m_outEdges.erase(&edge) > 0;
		FDK_ASSERT(result);
		delete &edge;
	}
}

#endif
