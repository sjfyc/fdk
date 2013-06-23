#include "Actor.h"
#include <fdk/Rand.h>
#include <fdkgame/NaviBlockMap.h>
#include <fdkgame/NaviVertexMap.h>
#include "Util.h"
#include "AStar.h"
#include "MapManager.h"

Actor::Actor(const Location& location, int moveCapability, int unitSize)
	: m_color(ARGB(180, (int)fdk::rand(100, 255), (int)fdk::rand(100, 255), (int)fdk::rand(100, 255)))
	, m_moveSpeed(100)
	, m_moveCapability(moveCapability)
	, m_unitSize(unitSize)
	, m_radius(unitSize*CELL_SIZE_X/2*0.75f)
	, m_location(location)	
	, m_velocity()
	, m_moveLocation()
	, m_astar(0)
{
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
	if ((m_moveLocation - m_location).length() <=  m_velocity.length()*delta)
	{
		m_location = m_moveLocation;
		m_moveLocation.reset();
		m_velocity.reset();
		return;
	}

	Location nextLocation = m_location + m_velocity*delta;
	CellRange nextCellRange = util::locationRangeToCellRange(
		LocationRange( nextLocation-Location(m_radius, m_radius)
			, nextLocation+Location(m_radius, m_radius) )
		);
	
	const fdkgame::navi::BlockMap& blockMap = g_MapManager.getBlockMap(m_moveCapability);
	for (short y = nextCellRange.topLeft.y; y <= nextCellRange.bottomRight.y; ++y)
	{
		for (short x = nextCellRange.topLeft.x; x <= nextCellRange.bottomRight.x; ++x)
		{
			if (blockMap.isBlock(CellCoord(x, y)) )
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
		}
	}

	m_location += m_velocity*delta;
}

bool Actor::searchPath(const Location& targetLocation)
{
	m_velocity.reset();
	m_moveLocation = m_location;

	m_astar = new AStar(*this, targetLocation);
	if (!m_astar->search())
	{
		util::output("actor can't find path");
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
