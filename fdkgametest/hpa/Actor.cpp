#include "Actor.h"
#include "HpaMap.h"
#include "Util.h"
#include <fdk/Rand.h>

Actor::Actor(const Location& location, float radius)
	: m_location(location)
	, m_color(ARGB(200, (int)fdk::rand(100, 255), (int)fdk::rand(100, 255), (int)fdk::rand(100, 255)))
	, m_velocity()
	, m_radius(radius)
	, m_moveLocation()
	, m_hpa(0)
{
}

Actor::~Actor()
{
	FDK_DELETE(m_hpa);
}

void Actor::tick(float delta)
{	
	if (m_velocity == Velocity::ZERO)
	{
		if (m_hpa)
		{
			int nodeID = m_hpa->popNextPathNode();
			if (nodeID == fdkgame::findpath::INVALID_NODEID)
			{
				fdkgame::findpath::Hpa::ErrorType error = m_hpa->getError();
				if (error == fdkgame::findpath::Hpa::Error_PathCompleted)
				{
					util::output("actor reached");
				}
				else
				{
					util::output("actor can't find path");
				}
				FDK_DELETE(m_hpa);
			}
			else
			{
				const int converNodes = (int)(m_radius*2/CELL_SIZE_X)+1;
				CellCoord coord = g_HpaMap.getLowLevelMap().toNodeCoord(nodeID);
				Location location = util::cellCoordToLocation(coord) + 
					Location((float)converNodes*CELL_SIZE_X/2, (float)converNodes*CELL_SIZE_Y/2);
				move(location, 100);
			}
		}
	}
	tickMove(delta);
}

void Actor::draw()
{
	g_HGE.Rectangle(m_location.x-m_radius, m_location.y-m_radius, m_location.x+m_radius, m_location.y+m_radius, m_color);
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
	m_location += m_velocity*delta;
}

void Actor::searchPath(const Location& targetLocation)
{	
	m_velocity.reset();
	m_moveLocation = m_location;

	const CellCoord& startCellCoord = util::locationToCellCoord(
		Location(m_location.x-m_radius, m_location.y-m_radius)
		);
	const CellCoord& targetCellCoord = util::locationToCellCoord(targetLocation);

	if (startCellCoord == targetCellCoord)
	{
		move(targetLocation, 100);
		return;
	}

	m_hpa = new Hpa(this, g_HpaMap, 
		g_HpaMap.getLowLevelMap().toNodeID(startCellCoord),
		g_HpaMap.getLowLevelMap().toNodeID(targetCellCoord));
	if (m_hpa->getError() == Hpa::Error_PathUnexist)
	{
		util::output("actor can't find path");
		FDK_DELETE(m_hpa);
		return;
	}
}

BoundingBox Actor::getBoundingBox() const
{
	return BoundingBox(m_location.x-m_radius,m_location.y-m_radius,
		m_location.x+m_radius+1,m_location.y+m_radius+1);
}
