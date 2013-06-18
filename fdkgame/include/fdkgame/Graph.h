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
		//friend class GraphNode<NodeInfoT, EdgeInfoT>;
		friend class Graph<NodeInfoT, EdgeInfoT>;
		typedef GraphNode<NodeInfoT, EdgeInfoT> NodeType;
	public:
		NodeType& getStartNode();
		const NodeType& getStartNode() const;
		NodeType& getTargetNode();
		const NodeType& getTargetNode() const;
		EdgeInfoT& getInfo();
		const EdgeInfoT& getInfo() const;
	private:
		GraphEdge(NodeType& startNode, NodeType& targetNode, const EdgeInfoT& info);
		NodeType* m_startNode;
		NodeType* m_targetNode;
		EdgeInfoT m_info;
	};

	template <class NodeInfoT, class EdgeInfoT>
	class GraphNode
	{
		friend class Graph<NodeInfoT, EdgeInfoT>;
		typedef GraphNode<NodeInfoT, EdgeInfoT> _Self;
	public:
		typedef GraphEdge<NodeInfoT, EdgeInfoT> EdgeType;
		struct CmpInEdge
		{
			bool operator()(const EdgeType* a, const EdgeType* b) const;
		};
		struct CmpOutEdge
		{
			bool operator()(const EdgeType* a, const EdgeType* b) const;
		};
		typedef std::set<const EdgeType*, CmpInEdge> InEdges;
		typedef std::set<const EdgeType*, CmpOutEdge> OutEdges;
		int getID() const;
		NodeInfoT& getInfo();
		const NodeInfoT& getInfo() const;	
		
		const InEdges& getInEdges() const;
		EdgeType* findInEdge(_Self& startNode);
		const EdgeType* findInEdge(_Self& startNode) const;
		EdgeType& getInEdge(_Self& startNode);
		const EdgeType& getInEdge(_Self& startNode) const;

		const OutEdges& getOutEdges() const;
		EdgeType* findOutEdge(_Self& targetNode);
		const EdgeType* findOutEdge(_Self& targetNode) const;
		EdgeType& getOutEdge(_Self& targetNode);
		const EdgeType& getOutEdge(_Self& targetNode) const;
	private:
		GraphNode(int id, const NodeInfoT& info);
		~GraphNode();
		int m_id;
		NodeInfoT m_info;
		InEdges m_inEdges;
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
		struct CmpEdge
		{
			bool operator()(const EdgeType* a, const EdgeType* b) const;
		};
		typedef std::set<const NodeType*, CmpNode> Nodes;
		typedef std::set<const EdgeType*, CmpEdge> Edges;
		Graph();
		~Graph();
		void clear();

		NodeType& addNode(int nodeID, const NodeInfoT& info);
		void removeNode(NodeType& node);
		
		EdgeType& addEdge(NodeType& startNode, NodeType& targetNode, const EdgeInfoT& info);
		void removeEdge(EdgeType& edge);
		
		const Nodes& getNodes() const;
		NodeType* findNode(int nodeID);
		const NodeType* findNode(int nodeID) const;
		NodeType& getNode(int nodeID);
		const NodeType& getNode(int nodeID) const;

		const Edges& getEdges() const;
		EdgeType* findEdge(NodeType& startNode, const NodeType& targetNode);
		const EdgeType* findEdge(NodeType& startNode, const NodeType& targetNode) const;
		EdgeType& getEdge(NodeType& startNode, const NodeType& targetNode);
		const EdgeType& getEdge(NodeType& startNode, const NodeType& targetNode) const;
	private:
		Nodes m_nodes;
		Edges m_edges;
	};

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>::GraphEdge(NodeType& startNode, NodeType& targetNode, const EdgeInfoT& info)
		: m_startNode(&startNode)
		, m_targetNode(&targetNode)
		, m_info(info)
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline int GraphEdge<NodeInfoT, EdgeInfoT>::getTargetNodeID() const
	{
		return m_targetNodeID;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>& GraphEdge<NodeInfoT, EdgeInfoT>::getStartNode()
	{
		FDK_ASSERT(m_startNode);
		return *m_startNode;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphNode<NodeInfoT, EdgeInfoT>& GraphEdge<NodeInfoT, EdgeInfoT>::getStartNode() const
	{
		FDK_ASSERT(m_startNode);
		return *m_startNode;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>& GraphEdge<NodeInfoT, EdgeInfoT>::getTargetNode()
	{
		FDK_ASSERT(m_targetNode);
		return *m_targetNode;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphNode<NodeInfoT, EdgeInfoT>& GraphEdge<NodeInfoT, EdgeInfoT>::getTargetNode() const
	{
		FDK_ASSERT(m_targetNode);
		return *m_targetNode;
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
	inline bool GraphNode<NodeInfoT, EdgeInfoT>::CmpInEdge::operator()(const EdgeType* a, const EdgeType* b) const
	{
		return a->m_startNode < b->m_startNode;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline bool GraphNode<NodeInfoT, EdgeInfoT>::CmpOutEdge::operator()(const EdgeType* a, const EdgeType* b) const
	{
		return a->m_targetNode < b->m_targetNode;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>::GraphNode(int id, const NodeInfoT& info)
		: m_id(id)
		, m_info(info)
		, m_inEdges()
		, m_outEdges()
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphNode<NodeInfoT, EdgeInfoT>::~GraphNode()
	{
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
	inline const typename GraphNode<NodeInfoT, EdgeInfoT>::InEdges& GraphNode<NodeInfoT, EdgeInfoT>::getInEdges() const
	{
		return m_inEdges;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findInEdge(_Self& startNode)
	{
		return const_cast<GraphEdge<NodeInfoT, EdgeInfoT>* >(
			static_cast<const _Self&>(*this).findInEdge(startNode)
			);
	}

	template <class NodeInfoT, class EdgeInfoT>
	const GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findInEdge(_Self& startNode) const
	{
		FDK_CMP_PTR(EdgeType, search);
		search->m_startNode = &startNode;
		InEdges::const_iterator it = m_inEdges.find(search);
		if (it == m_inEdges.end())
		{
			return NULL;
		}
		return *it;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getInEdge(_Self& startNode)
	{
		EdgeType* edge = findInEdge(startNode);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getInEdge(_Self& startNode) const
	{
		const EdgeType* edge = findInEdge(startNode);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const typename GraphNode<NodeInfoT, EdgeInfoT>::OutEdges& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdges() const
	{
		return m_outEdges;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findOutEdge(_Self& targetNode)
	{
		return const_cast<GraphEdge<NodeInfoT, EdgeInfoT>* >(
			static_cast<const _Self&>(*this).findOutEdge(targetNode)
			);
	}

	template <class NodeInfoT, class EdgeInfoT>
	const GraphEdge<NodeInfoT, EdgeInfoT>* GraphNode<NodeInfoT, EdgeInfoT>::findOutEdge(_Self& targetNode) const
	{
		FDK_CMP_PTR(EdgeType, search);
		search->m_targetNode = &targetNode;
		OutEdges::const_iterator it = m_outEdges.find(search);
		if (it == m_outEdges.end())
		{
			return NULL;
		}
		return *it;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdge(_Self& targetNode)
	{
		EdgeType* edge = findOutEdge(targetNode);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphEdge<NodeInfoT, EdgeInfoT>& GraphNode<NodeInfoT, EdgeInfoT>::getOutEdge(_Self& targetNode) const
	{
		const EdgeType* edge = findOutEdge(targetNode);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline bool Graph<NodeInfoT, EdgeInfoT>::CmpNode::operator()(const NodeType* a, const NodeType* b) const
	{
		return a->getID() < b->getID();
	}
	
	template <class NodeInfoT, class EdgeInfoT>
	inline bool Graph<NodeInfoT, EdgeInfoT>::CmpEdge::operator()(const EdgeType* a, const EdgeType* b) const
	{
		if (a->m_startNode != b->m_startNode)
		{
			return a->m_startNode < b->m_startNode;
		}
		return a->m_targetNode < b->m_targetNode;
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
		for (Edges::iterator it = m_edges.begin(); it != m_edges.end(); ++it)
		{
			delete *it;
		}
		m_edges.clear();
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

		while (!node.getOutEdges().empty())
		{
			EdgeType* edge = node.getOutEdges().begin();
			removeEdge(*edge);
		}

		while (!node.getInEdges().empty())
		{
			EdgeType* edge = node.getInEdges().begin();
			removeEdge(*edge);
		}

		bool result = m_nodes.erase(&node) > 0;
		FDK_ASSERT(result);
		delete &node;
	}

	template <class NodeInfoT, class EdgeInfoT>
	GraphEdge<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::addEdge(NodeType& startNode, NodeType& targetNode, const EdgeInfoT& info)
	{
		FDK_ASSERT(findNode(startNode.getID()) == &startNode);
		FDK_ASSERT(findNode(targetNode.getID()) == &targetNode);
		FDK_ASSERT(!findEdge(startNode, targetNode));

		EdgeType* edge = new EdgeType(startNode, targetNode, info);		
		bool result = startNode.m_outEdges.insert(edge).second;
		FDK_ASSERT(result);
		targetNode.m_inEdges.insert(edge).second;
		FDK_ASSERT(result);
		result = m_edges.insert(edge).second;
		FDK_ASSERT(result);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	void Graph<NodeInfoT, EdgeInfoT>::removeEdge(EdgeType& edge)
	{
		NodeType& startNode = edge.getStartNode();
		NodeType& targetNode = edge.getTargetNode();
		FDK_ASSERT(findEdge(startNode, targetNode) == &edge);

		bool result = m_edges.erase(&edge) > 0;
		FDK_ASSERT(result);		
		targetNode.m_inEdges.erase(&edge) > 0;
		FDK_ASSERT(result);
		startNode.m_outEdges.erase(&edge) > 0;
		FDK_ASSERT(result);
		delete &edge;
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
	inline const typename Graph<NodeInfoT, EdgeInfoT>::Edges& Graph<NodeInfoT, EdgeInfoT>::getEdges() const
	{
		return m_edges;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>* Graph<NodeInfoT, EdgeInfoT>::findEdge(NodeType& startNode, const NodeType& targetNode)
	{
		return const_cast<GraphEdge<NodeInfoT, EdgeInfoT>* >(
			static_cast<const _Self&>(*this).findEdge(startNode, targetNode)
			);
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphEdge<NodeInfoT, EdgeInfoT>* Graph<NodeInfoT, EdgeInfoT>::findEdge(NodeType& startNode, const NodeType& targetNode) const
	{
		FDK_CMP_PTR(EdgeType, search);
		search->m_startNode = &startNode;
		search->m_targetNode = &targetNode;
		Edges::const_iterator it = m_edges.find(search);
		if (it == m_edges.end())
		{
			return NULL;
		}
		return *it;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::getEdge(NodeType& startNode, const NodeType& targetNode)
	{
		EdgeType* edge = findEdge(startNode, targetNode);
		FDK_ASSERT(edge);
		return *edge;
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline const GraphEdge<NodeInfoT, EdgeInfoT>& Graph<NodeInfoT, EdgeInfoT>::getEdge(NodeType& startNode, const NodeType& targetNode) const
	{
		const EdgeType* edge = findEdge(startNode, targetNode);
		FDK_ASSERT(edge);
		return *edge;
	}
}}

#endif
