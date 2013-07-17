#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <set>
#include <fdkgame/NaviGridEnvUtil.h>
#include <fdkgame/NaviAStar.h>
#include <fdkgame/NaviJps.h>
#include <fdkgame/NaviJpsPlus.h>
#include <fdkgame/NaviVertexMap.h>
#include "Types.h"
class Actor;

class LocationPop;
class AStar 	
{
public:
	AStar(Actor& actor, const Location& targetLocation, bool bRefind);
	~AStar();
	bool search();
	size_t getPathLocationCount() const;
	bool popNextPathLocation(Location& location);
	void render();
private:
	bool _popNextPathLocation(Location& location);
	void doPartialFind(int startVertexID);
	const fdkgame::navi::VertexMap& getVertexMap() const;
	fdkgame::navi::AStar* newNavigator(const fdkgame::navi::VertexMap& vertexMap, int startVertexID, int targetVertexID);
	typedef fdkgame::navi::AStar Navigator;
	typedef std::set<VertexCoord> Vertexs;
	Actor& m_actor;
	Location m_targetLocation;
	
	bool m_bRefind;
	fdkgame::navi::AStar* m_navigator;
	std::vector<VertexCoord> m_vertexCoordPath;
	std::list<Location> m_locationPath;// 实际行走路线
	LocationPop* m_locationPop;
	
	bool m_hasNextPartial;
	int m_nextPartialStartID;
};

#endif