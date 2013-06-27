#include <vector>
#include <time.h>
#include <windows.h>
#include <fdkgame/NaviAStar.h>
#include <fdkgame/NaviJps.h>
#include <fdkgame/NaviMapManager.h>
#include <fdkgame/NaviTileMap.h>
#include <fdkgame/NaviVertexMap.h>
#include <fdk/Rand.h>

using namespace fdkgame;

namespace TestNavi
{
	enum TerrainType
	{
		TerrainType_None,
		TerrainType_Block,
	};

	void setObstacles(navi::TileMap& tileMap, int percent)
	{
		tileMap.reset(tileMap.getSizeX(), tileMap.getSizeY(), TerrainType_None);
		for (size_t y = 0; y < tileMap.getSizeY(); ++y)
		{
			for (size_t x = 0; x < tileMap.getSizeY(); ++x)
			{
				if (fdk::rand(1, 100) <= percent)
				{
					tileMap.setTileType(navi::CellCoord(x, y), TerrainType_Block);
				}			
			}
		}
	}

	static double getSecondsPerCycle()
	{
		LARGE_INTEGER frequency;
		BOOL result = QueryPerformanceFrequency(&frequency);
		FDK_ASSERT(result);
		return 1.0 / frequency.QuadPart;
	}

	double getSeconds()
	{
		static double secondsPerCycle = getSecondsPerCycle();
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		// Add big number to make bugs apparent where return value is being passed to FLOAT
		return cycles.QuadPart * secondsPerCycle + 16777216.0;
	}

	void runAStar(const navi::VertexMap& map, const std::vector<std::pair<int, int> >& startTargets)
	{
		for (size_t i = 0; i < startTargets.size(); ++i)
		{
			navi::AStar astar(map, startTargets[i].first, startTargets[i].second);
			astar.search();
		}
	}

	void runJps(const navi::VertexMap& map, const std::vector<std::pair<int, int> >& startTargets)
	{
		for (size_t i = 0; i < startTargets.size(); ++i)
		{
			navi::Jps jps(map, startTargets[i].first, startTargets[i].second);
			jps.search();
		}
	}
}

void testNavi()
{
	using namespace TestNavi;

	::srand((unsigned int)time(0));

	navi::TileMap tileMap;
	tileMap.reset(512, 512, 0);
	setObstacles(tileMap, 30);

	std::set<navi::MoveCapability> moveCapabilities;
	navi::MoveCapability moveCapability = 0;
	fdk::setEnumMask(moveCapability, TerrainType_None);
	moveCapabilities.insert(moveCapability);

	navi::MapManager mm(tileMap, moveCapabilities, 1, 1);
	mm.rebuildAfterTileMapReset();

	const navi::VertexMap& vertexMap = mm.getVertexMap(moveCapability, 1);

	// 产生10000个可以寻路的开始点和目标点

	std::vector<std::pair<int, int> > startTargets;
	size_t max_count = 100;
	size_t count = 0;
	while (count < max_count)
	{
		int startNodeID = fdk::rand(0, vertexMap.getNodeSpaceSize()-1);
		int targetNodeID = fdk::rand(0, vertexMap.getNodeSpaceSize()-1);
		if (startNodeID == targetNodeID)
		{
			continue;
		}
		if (!vertexMap.isNodeReachable(startNodeID) || 
			!vertexMap.isNodeReachable(targetNodeID) ) 
		{
			continue;
		}
		navi::AStar astar(vertexMap, startNodeID, targetNodeID);
		if (astar.search() != navi::AStar::SearchResult_Completed)
		{
			continue;
		}
		startTargets.push_back(std::make_pair(startNodeID, targetNodeID));
		++count;
	}

	{
		double searchStartTime = getSeconds();
		runAStar(vertexMap, startTargets);
		double searchEndTime = getSeconds();
		printf("astar cost %lf seconds\n", ((searchEndTime - searchStartTime)/startTargets.size()) );
	}
	{
		double searchStartTime = getSeconds();
		runJps(vertexMap, startTargets);
		double searchEndTime = getSeconds();
		printf("jps cost %lf seconds\n", ((searchEndTime - searchStartTime)/startTargets.size()) );
	}
}
	