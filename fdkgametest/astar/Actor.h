#ifndef __ACTOR_H_INCLUDE__
#define __ACTOR_H_INCLUDE__
#include "Types.h"

class Actor
{
	friend class ActorBank;
public:	
	void tick(float delta);
	void draw();
	void move(Location& location, float speed);
	const Location& getLocation() const;
	const Velocity& getVelocity() const;
	float getRadius() const;
private:
	Actor(const Location& location, float radius);
	~Actor();
	void tickMove(float delta);
	Location m_location;	
	Velocity m_velocity;
	float m_radius;
	Location m_moveLocation;
};

inline const Location& Actor::getLocation() const
{
	return m_location;
}

inline const Velocity& Actor::getVelocity() const
{
	return m_velocity;
}

inline float Actor::getRadius() const
{
	return m_radius;
}

#endif
