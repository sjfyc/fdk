#ifndef __ACTOR_H_INCLUDE__
#define __ACTOR_H_INCLUDE__
#include "Types.h"
#include <fdkgame/Math.h>
class AStar;

class Actor
{
	friend class ActorBank;
public:	
	void tick(float delta);
	void draw();
	void stopMove();
	void move(const Location& location, float speed);
	void teleport(const Location& location);
	int getID() const;
	float getRadius() const;
	int getMoveCapability() const;
	int getUnitSize() const;
	const Location& getLocation() const;
	const Velocity& getVelocity() const;	
	BoundingBox getBoundingBox() const;
	bool searchPath(const Location& targetLocation, bool bRefind);
	const Location& getLastAstarTargetLocation() const;
private:
	Actor(const Location& location, int moveCapability, int unitSize);
	~Actor();
	void tickMove(float delta);
	void drawOccupiedVertexs();
	int m_id;
	DWORD m_color;
	int m_moveCapability;
	int m_unitSize;	
	float m_radius;
	float m_moveSpeed;
	Location m_location;	
	Velocity m_velocity;	
	Location m_moveLocation;	
	AStar* m_astar;
	Location m_lastAstarTargetLocation;
};

inline int Actor::getMoveCapability() const
{
	return m_moveCapability;
}

inline int Actor::getUnitSize() const
{
	return  m_unitSize;
}

inline float Actor::getRadius() const
{
	return m_radius;
}

inline const Location& Actor::getLocation() const
{
	return m_location;
}

inline const Velocity& Actor::getVelocity() const
{
	return m_velocity;
}

inline const Location& Actor::getLastAstarTargetLocation() const
{
	return m_lastAstarTargetLocation;
}

#endif
