#include "Actor.h"
#include <fdk/Rand.h>
#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Option.h"
#include "Font.h"
#include "AStar.h"
#include "Game.h"
#include "MapManager.h"
#include "ActorBank.h"

Actor::Actor(const Location& location, int moveCapability, int unitSize)
	: m_color(ARGB(180, (int)fdk::rand(100, 255), (int)fdk::rand(100, 255), (int)fdk::rand(100, 255)))
	, m_moveSpeed(100)
	, m_moveCapability(moveCapability)
	, m_unitSize(unitSize)
	, m_radius(unitSize*HALF_CELL_SIZE_X*1.f)
	, m_location(location)	
	, m_velocity()
	, m_moveLocation()
	, m_astar(0)
{
	static int id = 0;
	m_id = id++;
}

Actor::~Actor()
{
}

void Actor::tick(float delta)
{	
	if (m_velocity == Velocity::ZERO)
	{
		if (m_astar)
		{
			Location location;
			if (!m_astar->popNextPathLocation(location))
			{
				util::output("actor reached");				
				FDK_DELETE(m_astar);
			}
			else
			{
				move(location, m_moveSpeed);
			}
		}
		else
		{
			if (g_Option.isOn(Option::Toggle_RandomMove))
			{
				Location loc(fdk::rand(CELL_SIZE_X*MAX_UNIT_SIZE,CELL_SIZE_X*CELL_COUNT_X-CELL_SIZE_X*MAX_UNIT_SIZE)*1.f, 
					fdk::rand(CELL_SIZE_Y*MAX_UNIT_SIZE,CELL_SIZE_Y*CELL_COUNT_Y-CELL_SIZE_Y*MAX_UNIT_SIZE)*1.f);
				searchPath(loc, false);
			}
		}
	}
	tickMove(delta);
}

void Actor::draw()
{	
	g_HGE.Rectangle(m_location.x-m_radius, m_location.y-m_radius, m_location.x+m_radius, m_location.y+m_radius, m_color);
	g_Font.printf(m_location.x-m_radius+2.0f,m_location.y-m_radius+2.0f,HGETEXT_LEFT,"%d",getID());
	g_HGE.Rectangle(m_location.x-1, m_location.y-1, m_location.x+1, m_location.y+1, MyColor_White);
	if (m_astar)
	{
		m_astar->render();
	}
	if (g_Option.isOn(Option::Toggle_ShowActorVertex))
	{
		drawOccupiedVertexs();
	}	
}

void Actor::stopMove()
{
	m_moveLocation.reset();
	m_velocity.reset();
	FDK_DELETE(m_astar);
	m_lastAstarTargetLocation.reset();
}

void Actor::move(const Location& location, float speed)
{
	m_velocity = (location - m_location).normalize()*speed;
	m_moveLocation = location;
}

void Actor::tickMove(float delta)
{
	if (m_velocity == Velocity::ZERO)
	{
		return;
	}
	
	bool bReachWayPoint = false;
	Location nextLocation = m_location + m_velocity*delta;
	if ((m_moveLocation - m_location).length() <=  m_velocity.length()*delta)
	{
		nextLocation = m_moveLocation;
		bReachWayPoint = true;
	}
		
	bool isLocationReachable = false;
	{		
		std::vector<fdkgame::navi::MapManager::PlotUnitArgument> plotArounds;
		std::vector<Actor*> aroundActors;
		g_ActorBank.getActors(aroundActors, nextLocation, getRadius()*8, this);
		for (size_t i = 0; i < aroundActors.size(); ++i)
		{
			Actor* aroundActor = aroundActors[i];
			fdkgame::navi::MapManager::PlotUnitArgument pua;
			pua.vertexCoord = util::locationToNearestVertexCoord(aroundActor->getLocation());
			pua.unitSize = aroundActor->getUnitSize();
			plotArounds.push_back(pua);
		}
		fdkgame::navi::MapManager::AutoPlotUnits _AutoPlotUnits(g_MapManager, plotArounds);

		isLocationReachable = g_MapManager.isLocationReachable(*this, nextLocation);
	}
	if (!isLocationReachable)
	{
		searchPath(m_lastAstarTargetLocation, true);
		return;
	}

	m_location = nextLocation;
	if (bReachWayPoint)
	{
		m_moveLocation.reset();
		m_velocity.reset();
		return;
	}
}

bool Actor::searchPath(const Location& targetLocation, bool bRefind)
{
	m_velocity.reset();
	m_moveLocation = m_location;

	m_astar = new AStar(*this, targetLocation, bRefind);
	if (!m_astar->search())
	{
		util::output("actor(%d) can't find path", getID());
		FDK_DELETE(m_astar);
		return false;
	}
	m_lastAstarTargetLocation = targetLocation;	
	return true;
}

BoundingBox Actor::getBoundingBox() const
{
	return BoundingBox(m_location.x-m_radius,m_location.y-m_radius,
		m_location.x+m_radius+1,m_location.y+m_radius+1);
}

int Actor::getID() const
{
	return m_id;
}

void Actor::drawOccupiedVertexs()
{	
	VertexCoord vertexCoord = util::locationToNearestVertexCoord(m_location);
	int totalExtend = g_Option.getUnitSize()+m_unitSize-1;
	for (int y = -totalExtend; y <= totalExtend; ++y)
	{
		for (int x = -totalExtend; x <= totalExtend; ++x)
		{
			VertexCoord aroundVertexCoord(vertexCoord.x+x, vertexCoord.y+y);
			util::drawVertex(aroundVertexCoord, ARGB(255, 226, 98, 29));
		}
	}
}

void Actor::teleport(const Location& location)
{
	stopMove();
	m_location = location;
}
