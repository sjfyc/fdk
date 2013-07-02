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
	//fdkgame::navi::MapManager::PlotUnitArgument subtract;
	//subtract.vertexCoord = util::locationToNearestVertexCoord(actor.getLocation());
	//subtract.unitSize = actor.getUnitSize();	
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

struct VertexMapRangeByCenter
	: public fdkgame::navi::GridPartEnv
{
	typedef fdkgame::navi::GridPartEnv _Base;
	VertexMapRangeByCenter(const fdkgame::navi::VertexMap& map, const VertexCoord& center)
		: _Base(map, _Base::Range::makeRectFromCenter( center, VertexCoord(40, 40) ) )
	{
	}
};

class LocationPop
{
	typedef fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode Poper;
	typedef fdkgame::navi::VertexMap Map;
public:
	LocationPop(
		const Map& map,
		bool bRefind,// refind时使用安全路线
		const Location& targetLocation
		)
		: m_map(map)
		, m_bRefind(bRefind)
		, m_targetLocation(targetLocation)
		, m_bTargetLocationUsed(false)
		, m_pop(0)
	{
	}
	~LocationPop()
	{
		delete m_pop;
	}
	std::list<int>& getRoughPathToInit() { return m_roughPath; }
	void afterRoughPathInited()
	{
		FDK_ASSERT(!m_pop);
		m_pop = new Poper(m_map, m_roughPath, false);
		if (!m_bRefind)
		{
			m_pop->pop(); // 忽略起点
		}
	}
	bool empty() const { return m_bTargetLocationUsed; }
	Location pop()
	{
		FDK_ASSERT(!empty());
		if (!m_pop || m_pop->empty())
		{
			m_bTargetLocationUsed = true;
			return m_targetLocation;
		}
		int nodeID = m_pop->pop();
		if (!m_bRefind && m_pop->empty()) // 忽略终点
		{
			return pop();
		}
		return util::vertexCoordToLocation(m_map.toNodeCoord(nodeID));
	}
private:
	std::list<int> m_roughPath;
	const Map& m_map;
	bool m_bRefind;	
	Location m_targetLocation;
	bool m_bTargetLocationUsed;
	Poper* m_pop;
};

AStar::AStar(Actor& actor, const Location& targetLocation, bool bRefind)
	: m_actor(actor)
	, m_targetLocation(targetLocation)
	, m_navigator(0)
	, m_bRefind(bRefind)
	, m_locationPop(0)
{	
}

AStar::~AStar()
{
	delete m_locationPop;
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
	int moveCapability = m_actor.getMoveCapability();
	fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, 
		fdkgame::navi::VertexMapType(m_actor.getMoveCapability(), m_actor.getUnitSize()),
		plotArounds);

	if (vertexMap.isBlock(startVertexCoord))
	{
		int middleNodeID = getFirstReachableNode(vertexMap, startVertexID, targetVertexID);
		if (middleNodeID == fdkgame::navi::INVALID_NODEID)
		{
			util::output("start vertex(%d/%d) is block & can't find a nonblock around",
				startVertexCoord.x, startVertexCoord.y);
			return false;
		}

		startVertexID = middleNodeID;
		startVertexCoord = vertexMap.toNodeCoord(startVertexID);
		m_actor.forceLocation(util::vertexCoordToLocation(startVertexCoord));
	}
	
	if (vertexMap.isBlock(targetVertexCoord) )
	{
		util::output("target vertex(%d/%d) is block",
			targetVertexCoord.x, targetVertexCoord.y);

		//int middleNodeID = getFirstDirectlyReachableNode(vertexMap, targetVertexID, startVertexID);
		//VertexMapRangeByCenter vertexMapRange(vertexMap, targetVertexCoord);
		int middleNodeID = getFirstReachableNode(vertexMap, targetVertexID, startVertexID);
		if (middleNodeID == fdkgame::navi::INVALID_NODEID)
		{
			util::output("target vertex(%d/%d) is block & can't find a nonblock around",
				targetVertexCoord.x, targetVertexCoord.y);
			return false;
		}

		targetVertexID = middleNodeID;
		targetVertexCoord = vertexMap.toNodeCoord(targetVertexID);
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
		m_locationPop = new LocationPop(vertexMap, m_bRefind, m_targetLocation);		
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
		m_locationPop = new LocationPop(vertexMap, m_bRefind, m_targetLocation);
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
	std::list<int> roughPath;
	m_navigator->getPath(roughPath, !m_bRefind ? true : false);

	{// 实际直连的行走路线
		if (!m_bRefind)
		{
			fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode directlyReachableNodes(vertexMap, roughPath, false);
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
			fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode directlyReachableNodes(vertexMap, roughPath, false);
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
		fdkgame::navi::GridEnvOctPathPopEachNode popEachNode(vertexMap, roughPath, false);
		while (!popEachNode.empty())
		{
			int nodeID = popEachNode.pop();			
			VertexCoord vertexCoord = vertexMap.toNodeCoord(nodeID);
			m_vertexCoordPath.push_back(vertexCoord);
		}
		m_vertexCoordPath.push_back(targetVertexCoord);
	}

	m_locationPop = new LocationPop(vertexMap, m_bRefind, m_targetLocation);
	m_navigator->getPath(m_locationPop->getRoughPathToInit(), !m_bRefind ? true : false);
	m_locationPop->afterRoughPathInited();

	return true;
}

bool AStar::popNextPathLocation(Location& location)
{
	//if (m_locationPath.empty())
	//{
	//	return false;
	//}
	//location = m_locationPath.front();
	//m_locationPath.pop_front();

	if (!m_locationPop || m_locationPop->empty())
	{
		return false;
	}
	location = m_locationPop->pop();
	return true;
}

size_t AStar::getPathLocationCount() const
{
	return m_locationPath.size();
}

const fdkgame::navi::VertexMap& AStar::getVertexMap() const
{
	return g_MapManager.getVertexMap(fdkgame::navi::VertexMapType(m_actor.getMoveCapability(), m_actor.getUnitSize()) );
}

