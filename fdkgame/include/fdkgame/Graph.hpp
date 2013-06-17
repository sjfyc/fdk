#ifndef __FDKGAME_GRAPH_HPP_INCLUDE__
#define __FDKGAME_GRAPH_HPP_INCLUDE__
#include "Base.h"
#include <set>
#include "GraphNode.h"

namespace fdk { namespace game
{
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
		EdgeType& addOutEdge(NodeType& startNode, const NodeType& targetNode, const EdgeInfoT& info);
	private:
		Nodes m_nodes;
	};	
}

#endif
