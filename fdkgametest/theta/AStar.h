#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <set>
#include <fdkgame/NaviAStar.h>
#include "Types.h"
class Actor;

class AStar 	
{
public:
	AStar(Actor& actor, const Location& targetLocation);
	~AStar();
	bool search();
	size_t getPathLocationCount() const;
	bool hasNextPathLocation() const;
	Location popNextPathLocation();
	void render();
private:
	typedef fdkgame::navi::AStar Navigator;
	typedef std::set<VertexCoord> Vertexs;
	std::vector<VertexCoord> m_vertexCoordPath;
	std::vector<Location> m_locationPath; 
	Actor& m_actor;

	fdkgame::navi::AStar* m_navigator;
	bool m_needSearch;
};

#endif