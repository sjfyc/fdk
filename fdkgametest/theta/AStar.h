#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <set>
#include <fdkgame/NaviGridEnvUtil.h>
#include <fdkgame/NaviAStar.h>
#include <fdkgame/NaviJps.h>
#include <fdkgame/NaviJpsPlus.h>
#include "Types.h"
class Actor;

class AStar 	
{
public:
	AStar(Actor& actor, const Location& targetLocation);
	~AStar();
	bool search();
	size_t getPathLocationCount() const;
	bool popNextPathLocation(Location& location);
	void render();
private:
	typedef fdkgame::navi::AStar Navigator;
	typedef std::set<VertexCoord> Vertexs;
	Actor& m_actor;
	Location m_targetLocation;
	fdkgame::navi::AStar* m_navigator;
	std::vector<VertexCoord> m_vertexCoordPath;
	std::vector<Location> m_locationPath;	
	fdkgame::navi::GridEnvOctPathPopDirectlyReachableNode* m_popDirectNode;
	Location* m_pTargetLocation;
	int m_startVertexID;
	int m_targetVertexID;
};

#endif