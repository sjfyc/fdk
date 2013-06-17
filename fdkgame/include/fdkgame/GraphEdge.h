#ifndef __FDKGAME_GRAPHEDGE_H_INCLUDE__
#define __FDKGAME_GRAPHEDGE_H_INCLUDE__
#include "GraphEdge.hpp"

namespace fdk { namespace game
{
	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>::GraphEdge(int targetNodeID, const EdgeInfoT& info)
		: m_targetNodeID(targetNodeID)
		, m_info(info)
	{
	}

	template <class NodeInfoT, class EdgeInfoT>
	inline GraphEdge<NodeInfoT, EdgeInfoT>::getTargetNodeID()
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
}}

#endif
