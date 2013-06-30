#include "AStar.h"
#include <fdkgame/NaviVertexMap.h>
#include <fdkgame/NaviGridEnvUtil.h>
#include "Util.h"
#include "Option.h"
#include "Game.h"
#include "Font.h"
#include "Actor.h"
#include "ActorBank.h"
#include "MapManager.h"

void plotAroundActors(Actor& actor, std::vector<fdkgame::navi::MapManager::PlotUnitArgument>& plotArounds)
{
	std::vector<Actor*> aroundActors;
	g_ActorBank.getActors(aroundActors, actor.getLocation(), (actor.getRadius()+(MAX_UNIT_SIZE+1)*HALF_CELL_SIZE_X)*1.5f, &actor);
	for (size_t i = 0; i < aroundActors.size(); ++i)
	{
		Actor* aroundActor = aroundActors[i];
		fdkgame::navi::MapManager::PlotUnitArgument pua;
		pua.vertexCoord = util::locationToNearestVertexCoord(aroundActor->getLocation());
		pua.unitSize = aroundActor->getUnitSize();
		plotArounds.push_back(pua);
	}
	fdkgame::navi::MapManager::PlotUnitArgument subtract;
	subtract.vertexCoord = util::locationToNearestVertexCoord(actor.getLocation());
	subtract.unitSize = actor.getUnitSize();	
}

VertexCoord simpleOffset(const VertexCoord& coord)
{
	VertexCoord result;
	if (coord.x > 0)
	{
		result.x = 1;
	}
	else if (coord.x < 0)
	{
		result.x = -1;
	}
	if (coord.y > 0)
	{
		result.y = 1;
	}
	else if (coord.y < 0)
	{
		result.y = -1;
	}
	return result;
}

AStar::AStar(Actor& actor, const Location& targetLocation, bool bRefind)
	: m_actor(actor)
	, m_targetLocation(targetLocation)
	, m_navigator(0)
	, m_bRefind(bRefind)
{	
}

AStar::~AStar()
{
	delete m_navigator;
}

void AStar::render()
{
	if (!g_Option.isOn(Option::Toggle_ShowPath))
	{
		return;
	}

	// 绘制实际行走路径
	Location prevLocation = m_actor.getLocation();
	for (std::list<Location>::iterator it = m_locationPath.begin(); it != m_locationPath.end(); ++it)
	{
		Location currentCenterLocation = *it;

		g_HGE->Gfx_RenderLine(prevLocation.x, prevLocation.y, 
			currentCenterLocation.x, currentCenterLocation.y,
			MyColor_Blue
			);
		prevLocation = currentCenterLocation;
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

	const fdkgame::navi::VertexMap& vertexMap = getVertexMap();

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

	int startVertexID = vertexMap.toNodeID(startVertexCoord);
	int targetVertexID = vertexMap.toNodeID(targetVertexCoord);
	
	std::vector<fdkgame::navi::MapManager::PlotUnitArgument> plotArounds;
	plotAroundActors(m_actor, plotArounds);
	fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, plotArounds);

	if (vertexMap.isBlock(startVertexCoord))
	{
		util::output("start vertex(%d/%d) is block, donothing for now", startVertexCoord.x, startVertexCoord.y);
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
		//m_locationPath.push_back(m_targetLocation);
		// 弹出来重新寻路
		return false;
	}
	
	if (vertexMap.isBlock(targetVertexCoord) )
	{
		util::output("target vertex(%d/%d) is block",
			targetVertexCoord.x, targetVertexCoord.y);

		int middleNodeID = getFirstDirectlyReachableNode(vertexMap, targetVertexID, startVertexID);
		if (middleNodeID == fdkgame::navi::INVALID_NODEID)
		{
			util::output("target vertex(%d/%d) is block & can't find a nonblock in line",
				targetVertexCoord.x, targetVertexCoord.y);
			return false;
		}

		targetVertexID = middleNodeID;
		targetVertexCoord = vertexMap.toNodeCoord(middleNodeID);
		m_targetLocation = util::vertexCoordToLocation(targetVertexCoord);
	}
	
	if (startVertexID == targetVertexID)
	{
		util::output("start vertex(%d/%d) equal to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		return true;
	}

	if (isDirectlyReachable(vertexMap, startVertexID, targetVertexID))
	{
		util::output("start vertex(%d/%d) can directly reach to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		return true;
	}
	
	std::list<int> pathWithOutStartTarget;
	if (g_Option.getNavigatorType() == Option::NavigatorType_AStar)
	{
		m_navigator = new fdkgame::navi::AStar(vertexMap, startVertexID, targetVertexID, pathWithOutStartTarget);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_Jps)
	{
		m_navigator = new fdkgame::navi::Jps(vertexMap, startVertexID, targetVertexID, pathWithOutStartTarget);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_JpsPlus)
	{
		m_navigator = new fdkgame::navi::JpsPlus(vertexMap, startVertexID, targetVertexID, pathWithOutStartTarget);
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

	{// 实际直连的行走路线
		if (!m_bRefind)
		{
			// 把起点终点加入产生一条由转向点构成的路径
			if (pathWithOutStartTarget.empty())
			{
				pathWithOutStartTarget.push_front(startVertexID);
				pathWithOutStartTarget.push_front(targetVertexID);
			}
			else if (pathWithOutStartTarget.size() >= 2)
			{
				VertexCoord dir1 = simpleOffset(
					vertexMap.toNodeCoord(*++pathWithOutStartTarget.begin()) - vertexMap.toNodeCoord(*pathWithOutStartTarget.begin())
					);
				
				if (simpleOffset( vertexMap.toNodeCoord(pathWithOutStartTarget.front()) - startVertexCoord ) == dir1)
				{
					pathWithOutStartTarget.pop_front();
					pathWithOutStartTarget.push_front(startVertexID);
				}
				
				VertexCoord dir2 = simpleOffset(
					vertexMap.toNodeCoord(pathWithOutStartTarget.back()) - vertexMap.toNodeCoord(* --(--pathWithOutStartTarget.end()) )
					);
				if (simpleOffset( targetVertexCoord - vertexMap.toNodeCoord(pathWithOutStartTarget.back() ) ) == dir2)
				{
					pathWithOutStartTarget.pop_back();
					pathWithOutStartTarget.push_back(targetVertexID);
				}
			}
			else
			{
				FDK_ASSERT(0);
			}
			
			fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode directlyReachableNodes(vertexMap, pathWithOutStartTarget, false);
			directlyReachableNodes.pop(); // 忽略起点
			
			while (!directlyReachableNodes.empty())
			{
				int nodeID = directlyReachableNodes.pop();
				if (!directlyReachableNodes.empty())  // 忽略终点
				{
					Location location = util::vertexCoordToLocation(vertexMap.toNodeCoord(nodeID));
					m_locationPath.push_back(location);	
				}
			}
		}
		else// refind时使用安全路线
		{
			fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode directlyReachableNodes(vertexMap, pathWithOutStartTarget, false);
			while (!directlyReachableNodes.empty())
			{
				int nodeID = directlyReachableNodes.pop();
				Location location = util::vertexCoordToLocation(vertexMap.toNodeCoord(nodeID));
				m_locationPath.push_back(location);	
			}
		}
		
		m_locationPath.push_back(m_targetLocation);
	}
		
	{// 精化为每个路点
		m_vertexCoordPath.push_back(startVertexCoord);
		std::vector<int> eachNodePath;
		eachNodePath.push_back(startVertexID);
		fdkgame::navi::GridEnvOctPathPopEachNode popEachNode(vertexMap, pathWithOutStartTarget, false);
		while (!popEachNode.empty())
		{
			int nodeID = popEachNode.pop();			
			VertexCoord vertexCoord = vertexMap.toNodeCoord(nodeID);
			m_vertexCoordPath.push_back(vertexCoord);
		}
		m_vertexCoordPath.push_back(targetVertexCoord);
	}

	return true;
}

bool AStar::popNextPathLocation(Location& location)
{
	if (m_locationPath.empty())
	{
		return false;
	}
	location = m_locationPath.front();
	m_locationPath.pop_front();
	return true;
}

size_t AStar::getPathLocationCount() const
{
	return m_locationPath.size();
}

const fdkgame::navi::VertexMap& AStar::getVertexMap() const
{
	return g_MapManager.getVertexMap(m_actor.getMoveCapability(), m_actor.getUnitSize());
}

