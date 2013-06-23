#include "AStar.h"
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Game.h"
#include "Font.h"
#include "Actor.h"
#include "ActorBank.h"
#include "MapManager.h"

AStar::AStar(Actor& actor, const Location& targetLocation)
	: m_actor(actor)
	, m_navigator(0)
	, m_needSearch(true)
{
	util::output("start path finding..");

	const Location& startLocation = actor.getLocation();		
	
	const VertexCoord& startVertexCoord = util::locationToVertexCoord(startLocation);
	const VertexCoord& targetVertexCoord = util::locationToVertexCoord(targetLocation);
	
	const fdkgame::navi::VertexMap& vertexMap = g_MapManager.getVertexMap(actor.getMoveCapability(), actor.getUnitSize());

	std::set<int> startVertexIDs;
	for (int yOffSet = 0; yOffSet <= 0; ++yOffSet)
	{
		for (int xOffSet = 0; xOffSet <= 0; ++xOffSet)
		{
			VertexCoord v = startVertexCoord+VertexCoord(xOffSet, yOffSet);
			if (!vertexMap.isValidVertexCoord(v) || vertexMap.isBlock(v))
			{
				continue;
			}
			startVertexIDs.insert(vertexMap.toVertexID(v));
		}
	}	
	if (startVertexIDs.empty())
	{
		m_needSearch = false;
		return;
	}
	
	m_locationPath.push_back(targetLocation);
	
	const int targetVertexID = vertexMap.toVertexID(targetVertexCoord);
	if (startVertexIDs.find(targetVertexID) != startVertexIDs.end())
	{
		m_needSearch = false;
		return;
	}

	// 考虑单位周围的单位
	m_navigator = new fdkgame::navi::AStar(vertexMap, startVertexIDs, targetVertexID);
}

AStar::~AStar()
{
	delete m_navigator;
}

void AStar::render()
{
	// 绘制路径
	if (!m_locationPath.empty())
	{
		Location prevLocation = m_actor.getLocation();
		for (int i = (int)m_locationPath.size()-1; i >= 0; --i)
		{
			Location currentCenterLocation = m_locationPath[i];

			g_HGE->Gfx_RenderLine(prevLocation.x, prevLocation.y, 
				currentCenterLocation.x, currentCenterLocation.y,
				MyColor_Blue
				);
			prevLocation = currentCenterLocation;
		}
	}
	// 绘制路径上的顶点
	for (size_t i = 0; i < m_vertexCoordPath.size(); ++i)
	{
		const VertexCoord& vertexCoord = m_vertexCoordPath[i];
		Location location = util::vertexCoordToLocation(vertexCoord);
		g_HGE.FrameRect(
			location.x-2, 
			location.y-2, 
			location.x+2, 
			location.y+2, 
			ARGB(255, 95, 26, 223)
			);
	}
}

bool AStar::search()
{	
	if (!m_needSearch)
	{
		return true;
	}
	
	std::vector<Actor*> aroundActors;
	g_ActorBank.getActors(aroundActors, m_actor.getLocation(), m_actor.getRadius()*8, &m_actor);
	struct PlotData
	{
		VertexCoord vertexCoord;
		int unitSize;
		bool xAlign;
		bool yAlign;
	};
	std::vector<PlotData> plotDatas;
	for (size_t i = 0; i < aroundActors.size(); ++i)
	{
		Actor* aroundActor = aroundActors[i];
		VertexCoord vertexCoord = util::locationToVertexCoord(aroundActor->getLocation());
		Location prunedlocation = util::vertexCoordToLocation(vertexCoord);
		fdkgame::VectorI iOrignLocation((int)aroundActor->getLocation().x, (int)aroundActor->getLocation().y);
		fdkgame::VectorI iPrunedlocation((int)prunedlocation.x, (int)prunedlocation.y);
		const bool xAlign = (iOrignLocation.x == iPrunedlocation.x);
		const bool yAlign = (iOrignLocation.y == iPrunedlocation.y);
		
		PlotData plotData;
		plotData.vertexCoord = vertexCoord;
		plotData.unitSize = aroundActor->getUnitSize();
		plotData.xAlign = xAlign;
		plotData.yAlign = yAlign;
		plotDatas.push_back(plotData);

		g_MapManager.plotUnit(plotData.vertexCoord, plotData.unitSize, plotData.xAlign, plotData.yAlign, true);
	}

	Navigator::SearchResult searchResult = m_navigator->search();

	for (size_t i = 0; i < plotDatas.size(); ++i)
	{
		const PlotData& plotData = plotDatas[i];
		g_MapManager.plotUnit(plotData.vertexCoord, plotData.unitSize, plotData.xAlign, plotData.yAlign, false);
	}

	if (searchResult == Navigator::SearchResult_PathUnexist)
	{
		return false;
	}
	FDK_ASSERT(searchResult == Navigator::SearchResult_Completed);
	
	const fdkgame::navi::VertexMap& vertexMap = static_cast<const fdkgame::navi::VertexMap&>(m_navigator->getEnvironment());
	const std::vector<int>& path = m_navigator->getPath();
	for (size_t i = 0; i < path.size(); ++i)
	{
		VertexCoord vertexCoord = vertexMap.toVertexCoord(path[i]);
		m_vertexCoordPath.push_back(vertexCoord);
		if (i >= 0)
		{
			Location location = util::vertexCoordToLocation(vertexCoord);
			m_locationPath.push_back(location);
		}		
	}

	return true;
}

bool AStar::hasNextPathLocation() const
{
	return !m_locationPath.empty();
}

Location AStar::popNextPathLocation()
{
	FDK_ASSERT(hasNextPathLocation());
	Location result = m_locationPath.back();
	m_locationPath.pop_back();
	return result;
}

size_t AStar::getPathLocationCount() const
{
	return m_locationPath.size();
}
