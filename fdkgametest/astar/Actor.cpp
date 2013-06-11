#include "Actor.h"

Actor::Actor(const Location& location, float radius)
	: m_location(location)
	, m_velocity()
	, m_radius(radius)
	, m_moveLocation()
{
}

Actor::~Actor()
{
}

void Actor::tick(float delta)
{	
	tickMove(delta);
}

void Actor::draw()
{
	g_HGE.Rectangle(m_location.x-m_radius, m_location.y-m_radius, m_location.x+m_radius, m_location.y+m_radius, 0xff101000);
}

void Actor::move(Location& location, float speed)
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
