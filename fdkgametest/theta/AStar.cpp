#include "AStar.h"
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Option.h"
#include "Game.h"
#include "Font.h"
#include "Actor.h"
#include "ActorBank.h"
#include "MapManager.h"

AStar::AStar(Actor& actor, const Location& targetLocation)
	: m_actor(actor)
	, m_targetLocation(targetLocation)
	, m_navigator(0)
{	
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
	else
	{
		g_HGE->Gfx_RenderLine(m_actor.getLocation().x, m_actor.getLocation().y, 
			m_targetLocation.x, m_targetLocation.y,
			MyColor_Blue
			);
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
	util::output("start path finding..");

	const fdkgame::navi::VertexMap& vertexMap = g_MapManager.getVertexMap(m_actor.getMoveCapability(), m_actor.getUnitSize());

	const Location& startLocation = m_actor.getLocation();
	VertexCoord startVertexCoord = util::locationToNearestVertexCoord(startLocation);
	VertexCoord targetVertexCoord = util::locationToNearestVertexCoord(m_targetLocation);	

	if (!vertexMap.isValidNodeCoord(startVertexCoord) ||
		!vertexMap.isValidNodeCoord(targetVertexCoord))
	{
		util::output("start vertex(%d/%d) or target vertex(%d/%d) invalid",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		return false;
	}

	std::vector<fdkgame::navi::MapManager::PlotUnitArgument> plotArounds;
	std::vector<Actor*> aroundActors;
	g_ActorBank.getActors(aroundActors, m_actor.getLocation(), m_actor.getRadius()*8, &m_actor);
	for (size_t i = 0; i < aroundActors.size(); ++i)
	{
		Actor* aroundActor = aroundActors[i];
		fdkgame::navi::MapManager::PlotUnitArgument pua;
		pua.vertexCoord = util::locationToNearestVertexCoord(aroundActor->getLocation());
		pua.unitSize = aroundActor->getUnitSize();
		plotArounds.push_back(pua);
	}
	fdkgame::navi::MapManager::PlotUnitArgument subtract;
	subtract.vertexCoord = util::locationToNearestVertexCoord(m_actor.getLocation());
	subtract.unitSize = m_actor.getUnitSize();	
	fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, plotArounds);
	
	if (vertexMap.isBlock(startVertexCoord))
	{
		util::output("start vertex(%d/%d) is block, bump out", startVertexCoord.x, startVertexCoord.y);
		//g_Game.pauseGame();

		//VertexCoord vertex;
		//vertexMap.getFirstReachableVertex(vertex, startVertexCoord, targetVertexCoord);
		//if ((vertex-startVertexCoord).length() > 10)
		//{
		//	return false;
		//}

		//else 
		//{
		//	// 直接弹出来
		//	m_actor.teleport(util::vertexCoordToLocation(vertex));
		//	startVertexCoord = vertex;
		//}
		//// when stuck with other unit, navigator will success with the target location returned
		m_locationPath.push_back(m_targetLocation);

		return true;
	}
	if (vertexMap.isBlock(targetVertexCoord) )
	{
		util::output("target vertex(%d/%d) is block",
			targetVertexCoord.x, targetVertexCoord.y);
		//g_Game.pauseGame();

		VertexCoord vertex;
		if (!vertexMap.getFirstReachableVertex(vertex, targetVertexCoord, startVertexCoord))
		{
			util::output("target vertex(%d/%d) is block & can't find a nonblock in line",
				targetVertexCoord.x, targetVertexCoord.y);
			return false;
		}
		targetVertexCoord = vertex;
		m_targetLocation = util::vertexCoordToLocation(targetVertexCoord);
	}

	int startVertexID = vertexMap.toNodeID(startVertexCoord);
	int targetVertexID = vertexMap.toNodeID(targetVertexCoord);

	m_locationPath.push_back(m_targetLocation);

	if (startVertexID == targetVertexID)
	{
		util::output("start vertex(%d/%d) equal to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		return true;
	}

	if (vertexMap.isDirectlyReachable(startVertexCoord, targetVertexCoord))
	{
		util::output("start vertex(%d/%d) can directly reach to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		return true;
	}
	
	if (g_Option.getNavigatorType() == Option::NavigatorType_AStar)
	{
		m_navigator = new fdkgame::navi::AStar(vertexMap, startVertexID, targetVertexID);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_Jps)
	{
		m_navigator = new fdkgame::navi::Jps(vertexMap, startVertexID, targetVertexID);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_JpsPlus)
	{
		m_navigator = new fdkgame::navi::JpsPlus(vertexMap, startVertexID, targetVertexID);
	}
	else 
	{
		FDK_ASSERT(0);
	}
	double searchStartTime = util::getSeconds();
	Navigator::SearchResult searchResult = m_navigator->search();
	double searchEndTime = util::getSeconds();
	util::output("search cost %lf seconds", searchEndTime - searchStartTime);
	if (searchResult == Navigator::SearchResult_PathUnexist)
	{
		util::output("no path between start vertex(%d/%d) and target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		return false;
	}

	FDK_ASSERT(searchResult == Navigator::SearchResult_Completed);
		
	const std::vector<int>& path = m_navigator->getPath();
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (i == 0 || i == path.size()-1)
		{
			continue;// ignore start vertex and target vertex
		}
		VertexCoord vertexCoord = vertexMap.toNodeCoord(path[i]);
		m_vertexCoordPath.push_back(vertexCoord);
		Location location = util::vertexCoordToLocation(vertexCoord);
		m_locationPath.push_back(location);	
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
