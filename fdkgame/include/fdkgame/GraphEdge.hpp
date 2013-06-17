#ifndef __FDKGAME_GRAPHEDGE_HPP_INCLUDE__
#define __FDKGAME_GRAPHEDGE_HPP_INCLUDE__
#include "Base.h"

namespace fdk { namespace game
{
	template <class NodeInfoT, class EdgeInfoT>
	class GraphNode;

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
}}

#endif
