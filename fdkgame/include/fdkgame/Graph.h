#ifndef __FDKGAME_GRAPH_H_INCLUDE__
#define __FDKGAME_GRAPH_H_INCLUDE__
#include "Base.h"
#include <set>

namespace fdk { namespace game
{
	template <class NodeInfoT, class EdgeInfoT>
	class GraphNode;

	template <class NodeInfoT, class EdgeInfoT>
	class GraphEdge;

	template <class NodeInfoT, class EdgeInfoT>
	class Graph;

	template <class NodeInfoT, class EdgeInfoT>
	class GraphEdge
	{
		friend class GraphNode<NodeInfoT, EdgeInfoT>;
	public:            
		int getTargetNodeID() const;
		EdgeInfoT& getInfo();
		const EdgeInfoT& getInfo() const;
	private:
		GraphEdge(int targetNodeID, const EdgeInfoT& info);
		int m_targetNodeID;
		EdgeInfoT m_info;
	};

	template <class NodeInfoT, class EdgeInfoT>
	class GraphNode
	{
		friend class Graph<NodeInfoT, EdgeInfoT>;
		typedef GraphNode<NodeInfoT, EdgeInfoT> _Self;
	public:
		typedef GraphEdge<NodeInfoT, EdgeInfoT> EdgeType;	
		struct CmpEdge
		{
			bool operator()(const EdgeType* a, const EdgeType* b) const;
		};
		typedef std::set<const EdgeType*, CmpEdge> OutEdges;
		int getID() const;
		NodeInfoT& getInfo();
		const NodeInfoT& getInfo() const;		
		const OutEdges& getOutEdges() const;
		EdgeType* findOutEdge(int targetNodeID);
		const EdgeType* findOutEdge(int targetNodeID) const;
		EdgeType& getOutEdge(int targetNodeID);
		const EdgeType& getOutEdge(int targetNodeID) const;
		void clearOutEdges();
		void removeOutEdge(EdgeType& edge);
	private:
		GraphNode(int id, const NodeInfoT& info);
		~GraphNode();
		EdgeType& addOutEdge(int targetNodeID, const EdgeInfoT& info);		
		int m_id;
		NodeInfoT m_info;
		OutEdges m_outEdges;
	};

	template <class NodeInfoT, class EdgeInfoT>
	class Graph
	{
		typedef Graph<NodeInfoT, EdgeInfoT> _Self;
	public:
		typedef GraphNode<NodeInfoT, EdgeInfoT> NodeType;
		typedef GraphEdge<NodeInfoT, EdgeInfoT> EdgeType;
		struct CmpNode
		{
			bool operator()(const NodeType* a, const NodeType* b) const;
		};
		typedef std::set<const NodeType*, CmpNode> Nodes;
		Graph();
		~Graph();
		void clear();
		NodeType& addNode(int nodeID, const NodeInfoT& info);
		void removeNode(NodeType& node);
		const Nodes& getNodes() const;
		NodeType* findNode(int nodeID);
		const NodeType* findNode(int nodeID) const;
		NodeType& getNode(int nodeID);
		const NodeType& getNode(int nodeID) const;
		EdgeType& addOutEdge(NodeType& startNode, const NodeType& targetNode, const EdgeInfoT& info);
	private:
		Nodes m_nodes;
	};

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>::GraphEdge(int targetNodeID, const EdgeInfoT& info)
		: m_targetNodeID(targetNodeID)
		, m_info(info)
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline int GraphEdge<NodeInfoT, EdgeInfoT>::getTargetNodeID() const
	{
		return m_targetNodeID;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline EdgeInfoT& GraphEdge<NodeInfoT, EdgeInfoT>::getInfo()
	{
		return m_info;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const EdgeInfoT& GraphEdge<NodeInfoT, EdgeInfoT>::getInfo() const
	{
		return m_info;
	}

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
	inline GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findOutEdge(int targetNodeID)
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
	inline GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdge(int targetNodeID)
	{
		EdgeType* edge = findOutEdge(targetNodeID);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdge(int targetNodeID) const
	{
		const EdgeType* edge = findOutEdge(targetNodeID);
		FDK_ASSERT(edge);
		return *edge;
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
	GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::addOutEdge(int targetNodeID, const EdgeInfoT& info)
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
	void Graph<NodeInfoT, EdgeInfoT>::clear()
	{
		for (Nodes::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			delete *it;
		}
		m_nodes.clear();
	}

	template <class NodeInfoT, class EdgeInfoT>
	GraphNode<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::addNode(int nodeID, const NodeInfoT& info)
	{
		FDK_ASSERT(!findNode(nodeID));
		NodeType* node = new NodeType(nodeID, info);
		bool result = m_nodes.insert(node).second;
		FDK_ASSERT(result);
		return *node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	void Graph<NodeInfoT, EdgeInfoT>::removeNode(NodeType& node)
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
	const GraphNode<NodeInfoT, EdgeInfoT>* Graph<NodeInfoT, EdgeInfoT>::findNode(int nodeID) const
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
	inline GraphNode<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::getNode(int nodeID)
	{
		NodeType* node = findNode(nodeID);
		FDK_ASSERT(node);
		return *node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphNode<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::getNode(int nodeID) const
	{
		const NodeType* node = findNode(nodeID);
		FDK_ASSERT(node);
		return *node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	GraphEdge<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::addOutEdge(NodeType& startNode, const NodeType& targetNode, const EdgeInfoT& info)
	{
		FDK_ASSERT(findNode(startNode.getID()) == &startNode);
		FDK_ASSERT(findNode(targetNode.getID()) == &targetNode);
		return startNode.addOutEdge(targetNode.getID(), info);
	}
}}

#endif
