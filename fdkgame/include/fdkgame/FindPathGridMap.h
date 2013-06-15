#ifndef __FDKGAME_FINDPATHGRIDMAP_H_INCLUDE__
#define __FDKGAME_FINDPATHGRIDMAP_H_INCLUDE__
#include "FindPathBase.h"
#include <fdk/Array2D.h>
#include "Math.h"

namespace fdk { namespace game { namespace findpath
{
	class FDKGAME_API GridMap
		: public Environment
	{
	public:
		struct Node
		{
			Node();
			bool bObstacle;
		};
		typedef Array2D<Node> MapData;
		void resetMap(size_t sizeX, size_t sizeY);
		const MapData& getMapData() const;
		void setObstacle(int nodeID, bool bSet=true);
		VectorI getNodeCoord(int nodeID) const;
		bool isValidNodeCoord(const VectorI& coord) const;
		int getNodeID(const VectorI& coord) const;
		// Environment interfaces
		virtual int getNodeCount() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	private:		
		MapData m_nodes;
	};

	inline GridMap::Node::Node()
		: bObstacle(false)
	{}

	inline void GridMap::resetMap(size_t sizeX, size_t sizeY)
	{
		m_nodes.reset(sizeX, sizeY);
	}

	inline const GridMap::MapData& GridMap::getMapData() const
	{
		return m_nodes;
	}

	inline void GridMap::setObstacle(int nodeID, bool bSet)
	{
		FDK_ASSERT(isValidNodeID(nodeID));
		m_nodes.raw_data()[nodeID].bObstacle = bSet;
	}
}}}

#endif
