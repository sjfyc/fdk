#ifndef __FDKGAME_GRAPHNODE_HPP_INCLUDE__
#define __FDKGAME_GRAPHNODE_HPP_INCLUDE__
#include "Base.h"
#include <set>

namespace fdk { namespace game
{
	template <class NodeInfoT, class EdgeInfoT>
	class Graph;

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
}

#endif
