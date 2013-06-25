#include "Actor.h"
#include <fdk/Rand.h>
#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "Font.h"
#include "AStar.h"
#include "MapManager.h"
#include "ActorBank.h"

struct AutoPlot// 考虑单位周围的单位
{
	AutoPlot(const Location& location, float radius, Actor* egnore) 
	{
		g_ActorBank.getActors(m_aroundActors, location, radius, egnore);
		for (size_t i = 0; i < m_aroundActors.size(); ++i)
		{
			Actor* aroundActor = m_aroundActors[i];
			aroundActor->plotToMapManager(true);
		}
	}

	~AutoPlot()
	{
		for (size_t i = 0; i < m_aroundActors.size(); ++i)
		{
			Actor* aroundActor = m_aroundActors[i];
			aroundActor->plotToMapManager(false);
		}
	}
	std::vector<Actor*> m_aroundActors;
};

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
			if (!m_astar->hasNextPathLocation())
			{
				util::output("actor reached");				
				FDK_DELETE(m_astar);
			}
			else
			{
				Location location = m_astar->popNextPathLocation();
				move(location, m_moveSpeed);
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
	
	if (isLocationBlocked(m_location))
	{
		stopMove();
		return;
	}

	bool bReachWayPoint = false;
	Location nextLocation = m_location + m_velocity*delta;
	if ((m_moveLocation - m_location).length() <=  m_velocity.length()*delta)
	{
		nextLocation = m_moveLocation;
		bReachWayPoint = true;
	}
	
	if (!g_MapManager.isLocationReachable(*this, nextLocation))
	{
		if (searchPath(m_lastAstarTargetLocation))
		{
			if (m_astar->getPathLocationCount() == 1)
			{
				util::output("stop move 1");
				stopMove();
			}
		}
		return;
	}

	{
		AutoPlot _autoPlot(nextLocation, getRadius()*8, this);
		// 考虑周围半径两个最大单位尺寸大小的人
		// 考察与每个单位之间的距离

		if (!g_MapManager.isLocationReachable(*this, nextLocation))
		{
			if (searchPath(m_lastAstarTargetLocation))
			{
				if (m_astar->getPathLocationCount() == 1)
				{
					util::output("stop move 2");
					stopMove();
				}
			}
			return;
		}
	}

	if (isLocationBlocked(nextLocation))
	{
		if (searchPath(m_lastAstarTargetLocation))
		{
			if (m_astar->getPathLocationCount() == 1)
			{
				stopMove();
			}
		}
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

bool Actor::isLocationBlocked(const Location& location) const
{
	return false;
	// 考虑周围的静态阻挡
	CellRange cellRange = util::locationRangeToCellRange(
		LocationRange( location-Location( float( (m_unitSize*CELL_SIZE_X-1)/2 ), float( (m_unitSize*CELL_SIZE_Y-1)/2 ) ), 
		location+Location( float( (m_unitSize*CELL_SIZE_X)/2 ), float((m_unitSize*CELL_SIZE_Y)/2 ) ) )
		);

	const fdkgame::navi::BlockMap& blockMap = g_MapManager.getBlockMap(m_moveCapability);
	for (short y = cellRange.topLeft.y; y <= cellRange.bottomRight.y; ++y)
	{
		for (short x = cellRange.topLeft.x; x <= cellRange.bottomRight.x; ++x)
		{
			if (blockMap.isBlock(CellCoord(x, y)) )
			{				
				return true;
			}
		}
	}

	// 考虑周围的动态阻挡
	if (g_ActorBank.getFirstCollideActor(location, m_radius, this))
	{
		return true;
	}

	return false;
}

bool Actor::searchPath(const Location& targetLocation)
{
	m_velocity.reset();
	m_moveLocation = m_location;

	m_astar = new AStar(*this, targetLocation);
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

void Actor::plotToMapManager(bool bPlot)
{
	VertexCoord vertexCoord = util::locationToVertexCoord(getLocation());
	Location prunedlocation = util::vertexCoordToLocation(vertexCoord);
	fdkgame::VectorI iOrignLocation((int)getLocation().x, (int)getLocation().y);
	fdkgame::VectorI iPrunedlocation((int)prunedlocation.x, (int)prunedlocation.y);
	const bool xAlign = (iOrignLocation.x == iPrunedlocation.x);
	const bool yAlign = (iOrignLocation.y == iPrunedlocation.y);

	g_MapManager.plotUnit(vertexCoord, getUnitSize(), xAlign, yAlign, bPlot);
}

int Actor::getID() const
{
	return m_id;
}
