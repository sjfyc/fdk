#include "AStar.h"
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Game.h"
#include "Font.h"
#include "Actor.h"
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
	Navigator::SearchResult searchResult = m_navigator->search();
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
