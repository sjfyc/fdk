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
		typedef VectorI NodeCoord;
		static const int COST_STRAIGHT = 100;	
		static const int COST_DIAGONAL = 142;
		void resetMap(size_t sizeX, size_t sizeY);
		const MapData& getMapData() const;
		void clearObstacles();
		void setObstacle(int nodeID, bool bSet=true);
		NodeCoord getNodeCoord(int nodeID) const;
		int getNodeID(const NodeCoord& coord) const;
		bool isValidNodeCoord(const NodeCoord& coord) const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	private:
		bool tryAddSuccessorNode(std::vector<SuccessorNodeInfo>& result, const NodeCoord& coord, int cost) const;
		MapData m_nodes;
	};

	class FDKGAME_API GridMapPart
		: public Environment
	{
	public:
		typedef GridMap::NodeCoord OrginNodeCoord;
		typedef Rect<GridMap::NodeCoord::ValueType> Range;
		typedef VectorI PartNodeCoord;		
		GridMapPart(GridMap& orignMap, const Range& range);
		GridMap& getOrignMap() const;
		const Range& getRange() const;
		PartNodeCoord getPartNodeCoord(int partNodeID) const;
		int getPartNodeID(const PartNodeCoord& partNodeCoord) const;
		bool isValidPartNodeCoord(const PartNodeCoord& partNodeCoord) const;
		// Environment interfaces
		virtual int getNodeSpaceSize() const;
		virtual int getHeuristic(int startNodeID, int targetNodeID) const;
		virtual void getSuccessorNodes(int nodeID, std::vector<SuccessorNodeInfo>& result) const;
		virtual bool isObstacle(int nodeID) const;
	private:		
		int toOrignNodeID(int partNodeID) const;
		OrginNodeCoord toOrignNodeCoord(const PartNodeCoord& partNodeCoord) const;
		int toPartNodeID(int orignNodeID) const;
		PartNodeCoord toPartNodeCoord(const OrginNodeCoord& orignNodeCoord) const;
		GridMap& m_orignMap;
		Range m_range;
	};

	inline GridMap::Node::Node()
		: bObstacle(false)
	{
	}

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
	
	inline GridMap& GridMapPart::getOrignMap() const
	{
		return m_orignMap;
	}

	inline const GridMapPart::Range& GridMapPart::getRange() const
	{
		return m_range;
	}

	inline GridMapPart::PartNodeCoord GridMapPart::getPartNodeCoord(int partNodeID) const
	{
		return PartNodeCoord(partNodeID%m_range.width(), partNodeID/m_range.width());
	}

	inline GridMapPart::OrginNodeCoord GridMapPart::toOrignNodeCoord(const PartNodeCoord& partNodeCoord) const
	{
		return partNodeCoord + m_range.topLeft;
	}

	inline GridMapPart::PartNodeCoord GridMapPart::toPartNodeCoord(const OrginNodeCoord& orignNodeCoord) const
	{
		return orignNodeCoord - m_range.topLeft;
	}
}}}

#endif
