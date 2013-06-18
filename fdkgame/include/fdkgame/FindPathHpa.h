#ifndef __FDKGAME_FINDPATHHPA_H_INCLUDE__
#define __FDKGAME_FINDPATHHPA_H_INCLUDE__
#include "FindPathBase.h"
#include "FindPathGridMap.h"

namespace fdk { namespace game { namespace findpath
{
	struct Entrance
	{
	public:
		int node1ID;
		int node2ID;
	};

	class AbstractGridMap
	{
	public:
		AbstractGridMap(GridMap& lowLevelMap, const VectorI& maxClusterSize);
		void buildEntrances();
	protected:
		void createHorizontalEntrances(int xStart, int xEnd, int y);
		void createVerticalEntrances(int yStart, int yEnd, int x);
		GridMap& m_lowLevelMap;
		const VectorI m_maxClusterSize;
		std::vector<Entrance> m_entrances;
	};
}}}

#endif
