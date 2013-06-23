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
	const Location& startLocation = actor.getLocation();
	m_locationPath.push_back(targetLocation);	
	
	const VertexCoord& startVertexCoord = util::locationToVertexCoord(startLocation);
	const VertexCoord& targetVertexCoord = util::locationToVertexCoord(targetLocation);
	
	const fdkgame::navi::VertexMap& vertexMap = g_MapManager.getVertexMap(actor.getMoveCapability(), actor.getUnitSize());
	int targetVertexID = vertexMap.toVertexID(targetVertexCoord);

	std::set<int> startVertexIDs;
	VertexCoord  startRightVertexCoord = startVertexCoord + VertexCoord(1, 0);
	VertexCoord  startBottomVertexCoord = startVertexCoord + VertexCoord(0, 1);
	VertexCoord  startBottomRightVertexCoord = startVertexCoord + VertexCoord(1, 1);
	
	startVertexIDs.insert(vertexMap.toVertexID(startRightVertexCoord));
	startVertexIDs.insert(vertexMap.toVertexID(startBottomVertexCoord));
	startVertexIDs.insert(vertexMap.toVertexID(startBottomRightVertexCoord));
	
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
	// »æÖÆÂ·¾¶
	if (hasNextPathLocation())
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
		if (i > 0)
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
