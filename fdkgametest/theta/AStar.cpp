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
#if 0
bool AStar::search()
{
	util::output("start path %sfinding..", m_bRefind?"re":"");

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

	// 起点是障碍，则将起点修正为距离终点较近的可站立点
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

	if (plotArounds.empty() || 
		(targetVertexCoord-startVertexCoord).lengthSquared() > getPlotActorRadiusInVertex()*getPlotActorRadiusInVertex()*1.44f
		)
	{// 没有任何动态障碍时，那么只考虑起点和目标点是否静态联通，如果不联通，则进行目标点修正
		if (!vertexMap.getConnectorComponent()->isConnected(startVertexCoord, targetVertexCoord))
		{
			util::output("target vertex(%d/%d) is in different static island",
				targetVertexCoord.x, targetVertexCoord.y);

			if (vertexMap.getConnectorComponent()->getConnectedNodeCount(startVertexCoord) > 
				vertexMap.getConnectorComponent()->getConnectedNodeCount(targetVertexCoord) )
			{// 终点处于封闭区域
				int middleNodeID = getFirstConnectedNode(vertexMap, targetVertexID, startVertexID);
				FDK_ASSERT(middleNodeID != fdkgame::navi::INVALID_NODEID);

				targetVertexID = middleNodeID;
				targetVertexCoord = vertexMap.toNodeCoord(targetVertexID);
				m_targetLocation = util::vertexCoordToLocation(targetVertexCoord);
			}
			else
			{// 起点处于封闭区域
				//不必调整目标
			}
		}
	}
	else
	{// 需要考虑动态障碍
		const fdkgame::navi::GridNodeRange considerRect(fdkgame::navi::GridNodeRange::makeRectFromCenter(
			startVertexCoord, VertexCoord(
			int(getPlotActorRadiusInVertex()*1.2f), 
			int(getPlotActorRadiusInVertex()*1.2f) )
			));

		while (1)
		{			
			int middleVertexID = getFirstConnectedNode(vertexMap, targetVertexID, startVertexID);
			FDK_ASSERT(middleVertexID != fdkgame::navi::INVALID_NODEID);
			const VertexCoord middleVertexCoord = vertexMap.toNodeCoord(middleVertexID);
			util::output("consider dynamic island, middle vertex[%d/%d] found", middleVertexCoord.x, middleVertexCoord.y);
			if (!considerRect.contain(vertexMap.toNodeCoord(middleVertexID)) ||
				!fillTempColorInClosedArea(vertexMap, middleVertexID, considerRect) ||
				vertexMap.getConnectorComponent()->isConnected(startVertexCoord, middleVertexCoord) )
			{
				targetVertexID = middleVertexID;
				targetVertexCoord = vertexMap.toNodeCoord(targetVertexID);
				m_targetLocation = util::vertexCoordToLocation(targetVertexCoord);
				break;
			}
		}
		vertexMap.getColorComponent()->clearTempColors();
	}

	if (startVertexID == targetVertexID)
	{
		util::output("start vertex(%d/%d) equal to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);		
		return true;
	}

	// 重新寻路时不走直连（不安全）
	if (!m_bRefind && isDirectlyReachable(vertexMap, startVertexID, targetVertexID))
	{
		util::output("start vertex(%d/%d) can directly reach to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);
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
		util::output("search failed, use closest target");
	}
	std::list<int> roughPath;
	m_navigator->getPath(roughPath, !m_bRefind ? Navigator::PathOption_WithStartTarget : 0);

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

		if (searchResult != Navigator::SearchResult_PathUnexist)
		{
			m_locationPath.push_back(m_targetLocation);
		}

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

	m_locationPop = new LocationPop(vertexMap, m_bRefind, 
		(searchResult != Navigator::SearchResult_PathUnexist) ? &m_targetLocation : 0);
	m_navigator->getPath(m_locationPop->getRoughPathToInit(), !m_bRefind ? Navigator::PathOption_WithStartTarget : 0);
	m_locationPop->afterRoughPathInited();

	return true;
}
#endif

void plotAroundActors(Actor& actor, std::vector<fdkgame::navi::MapManager::PlotUnitArgument>& plotArounds)
{
	std::vector<Actor*> aroundActors;
	g_ActorBank.getActors(aroundActors, actor.getLocation(), getPlotActorRadius(), &actor);
	for (size_t i = 0; i < aroundActors.size(); ++i)
	{
		Actor* aroundActor = aroundActors[i];
		fdkgame::navi::MapManager::PlotUnitArgument pua;
		pua.vertexCoord = util::locationToNearestVertexCoord(aroundActor->getLocation());
		pua.unitSize = aroundActor->getUnitSize();
		plotArounds.push_back(pua);
	}
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
		const Location* targetLocation
		)
		: m_map(map)
		, m_bRefind(bRefind)
		, m_targetLocation(targetLocation?*targetLocation:Location())
		, m_bTargetLocationUsed(false)
		, m_bEnableTargetLocation(targetLocation?true:false)
		, m_pop(0)
		, m_forceStartNodeID(fdkgame::navi::INVALID_NODEID)
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
			int nodeID = m_pop->pop(); // 忽略起点
			if (m_pop->empty() && !m_bEnableTargetLocation)
			{
				m_forceStartNodeID = nodeID;
			}
		}
	}
	bool empty() const 
	{ 
		if (m_forceStartNodeID != fdkgame::navi::INVALID_NODEID)
		{
			return false;
		}
		return m_bEnableTargetLocation ? m_bTargetLocationUsed : (!m_pop || m_pop->empty()); 
	}
	Location pop()
	{
		FDK_ASSERT(!empty());
		if (m_forceStartNodeID != fdkgame::navi::INVALID_NODEID)
		{
			int nodeID = m_forceStartNodeID;
			m_forceStartNodeID = fdkgame::navi::INVALID_NODEID;
			return util::vertexCoordToLocation(m_map.toNodeCoord(nodeID));
		}
		if (!m_pop || m_pop->empty())
		{
			m_bTargetLocationUsed = true;
			return m_targetLocation;
		}
		int nodeID = m_pop->pop();
		if (!m_bRefind && m_pop->empty()) // 忽略终点
		{
			if (m_bEnableTargetLocation)
			{
				return pop();
			}
		}
		return util::vertexCoordToLocation(m_map.toNodeCoord(nodeID));
	}
private:
	std::list<int> m_roughPath;
	const Map& m_map;
	bool m_bRefind;	
	Location m_targetLocation;
	bool m_bTargetLocationUsed;
	bool m_bEnableTargetLocation;
	Poper* m_pop;
	int m_forceStartNodeID;
};

AStar::AStar(Actor& actor, const Location& targetLocation, bool bRefind)
	: m_actor(actor)
	, m_targetLocation(targetLocation)
	, m_navigator(0)
	, m_bRefind(bRefind)
	, m_locationPop(0)
	, m_hasNextPartial(false)
	, m_nextPartialStartID(fdkgame::navi::INVALID_NODEID)
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
	util::output("start path %sfinding..", m_bRefind?"re":"");

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

	if ((m_actor.getLocation()-m_targetLocation).isNearlyZero())
	{
		util::output("start location equal to targetlocation, no need to move");
		return true;
	}

	int startVertexID = vertexMap.toNodeID(startVertexCoord);
	int targetVertexID = vertexMap.toNodeID(targetVertexCoord);

	if (startVertexID == targetVertexID)
	{
		util::output("start vertex(%d/%d) equal to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		//
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		//
		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);
		return true;
	}

	std::vector<fdkgame::navi::MapManager::PlotUnitArgument> plotArounds;
	plotAroundActors(m_actor, plotArounds);
	int moveCapability = m_actor.getMoveCapability();
	fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, 
		fdkgame::navi::VertexMapType(m_actor.getMoveCapability(), m_actor.getUnitSize()),
		plotArounds);

	// 起点和目标点几乎相同
	// 起点是障碍，则将起点修正为距离终点较近的可站立点
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

	if (startVertexID == targetVertexID)
	{
		util::output("start vertex(%d/%d) equal to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);

		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);

		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);
		return true;
	}

	doPartialFind(startVertexID);
	return true;
}

void AStar::doPartialFind(int startVertexID)
{
	const fdkgame::navi::VertexMap& vertexMap = getVertexMap();
	VertexCoord startVertexCoord = vertexMap.toNodeCoord(startVertexID);
	VertexCoord targetVertexCoord = util::locationToNearestVertexCoord(m_targetLocation);
	int targetVertexID = vertexMap.toNodeID(targetVertexCoord);

	// 重新寻路时不走直连（不安全）
	if (!m_bRefind && isDirectlyReachable(vertexMap, startVertexID, targetVertexID))
	{
		util::output("start vertex(%d/%d) can directly reach to target vertex(%d/%d)",
			startVertexCoord.x, startVertexCoord.y,
			targetVertexCoord.x, targetVertexCoord.y);
		//<< path drawing info
		m_vertexCoordPath.push_back(startVertexCoord);
		m_vertexCoordPath.push_back(targetVertexCoord);
		m_locationPath.push_back(m_targetLocation);
		//>>
		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);
		return;
	}

	// do find path using navigator
	m_navigator = newNavigator(vertexMap, startVertexID, targetVertexID);
	double searchStartTime = util::getSeconds();
	Navigator::SearchResult searchResult = m_navigator->search(1500);
	double searchEndTime = util::getSeconds();
	util::output("search cost %lf seconds", searchEndTime - searchStartTime);

	if (searchResult == Navigator::SearchResult_Completed)
	{// 直接找到目标
		m_locationPop = new LocationPop(vertexMap, m_bRefind, &m_targetLocation);
		m_navigator->getPath(m_locationPop->getRoughPathToInit(), !m_bRefind ? Navigator::PathOption_WithStartTarget : 0);
		m_locationPop->afterRoughPathInited();
	}
	else
	{
		int closestVertexID = m_navigator->getClosestNodeID();
		if (closestVertexID == startVertexID)
		{
			util::output("the partial find returns same result as prev find, break finding");
		}
		else
		{
			m_locationPop = new LocationPop(vertexMap, m_bRefind, 0);
			m_navigator->getPath(m_locationPop->getRoughPathToInit(), !m_bRefind ? Navigator::PathOption_WithStartTarget : 0);
			m_locationPop->afterRoughPathInited();

			m_hasNextPartial = true;
			m_nextPartialStartID = closestVertexID;
			util::output("partial find to middle vertex(%d/%d)", 
				vertexMap.toNodeCoord(closestVertexID).x, 
				vertexMap.toNodeCoord(closestVertexID).y);
		}
	}
}

bool AStar::popNextPathLocation(Location& location)
{
	std::vector<fdkgame::navi::MapManager::PlotUnitArgument> plotArounds;
	plotAroundActors(m_actor, plotArounds);
	int moveCapability = m_actor.getMoveCapability();
	fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, 
		fdkgame::navi::VertexMapType(m_actor.getMoveCapability(), m_actor.getUnitSize()),
		plotArounds);

	if (_popNextPathLocation(location))
	{
		return true;
	}
	if (!m_hasNextPartial)
	{
		return false;
	}	
	m_hasNextPartial = false;	
	FDK_DELETE(m_locationPop);
	FDK_DELETE(m_navigator);
	doPartialFind(m_nextPartialStartID);
	return popNextPathLocation(location);
}

bool AStar::_popNextPathLocation(Location& location)
{
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

fdkgame::navi::AStar* AStar::newNavigator(const fdkgame::navi::VertexMap& vertexMap, int startVertexID, int targetVertexID)
{
	if (g_Option.getNavigatorType() == Option::NavigatorType_AStar)
	{
		return new fdkgame::navi::AStar(vertexMap, startVertexID, targetVertexID);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_Jps)
	{
		return new fdkgame::navi::Jps(vertexMap, startVertexID, targetVertexID);
	}
	else if (g_Option.getNavigatorType() == Option::NavigatorType_JpsPlus)
	{
		return new fdkgame::navi::JpsPlus(vertexMap, startVertexID, targetVertexID);
	}
	else 
	{
		FDK_ASSERT(0);
		return 0;
	}
}
