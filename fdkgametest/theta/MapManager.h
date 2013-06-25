#ifndef __MapManager_H_INCLUDE__
#define __MapManager_H_INCLUDE__
#include "Types.h"
#include <fdkgame/NaviMapManager.h>
class Actor;

class MapManager
	: public fdk::Singleton<MapManager>
	, public fdkgame::navi::MapManager
{
	friend class fdk::Singleton<MapManager>;
	typedef fdkgame::navi::MapManager _Base;
public:		
	void draw();
	void drawCellInfo();
	bool isLocationReachable(Actor& actor, const Location& location);
private:
	MapManager();
	~MapManager();
};

#define g_MapManager (MapManager::instance())
#endif
