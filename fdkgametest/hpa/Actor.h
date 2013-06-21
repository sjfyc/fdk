#ifndef __ACTOR_H_INCLUDE__
#define __ACTOR_H_INCLUDE__
#include "Types.h"
#include <fdkgame/Math.h>
class Hpa;

class Actor
{
	friend class ActorBank;
public:	
	void tick(float delta);
	void draw();
	void move(const Location& location, float speed);
	const Location& getLocation() const;
	const Velocity& getVelocity() const;
	float getRadius() const;
	void searchPath(const Location& targetLocation);
	BoundingBox getBoundingBox() const;
private:
	Actor(const Location& location, float radius);
	~Actor();
	void tickMove(float delta);
	DWORD m_color;
	Location m_location;	
	Velocity m_velocity;
	float m_radius;
	Location m_moveLocation;
	Hpa* m_hpa;
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
