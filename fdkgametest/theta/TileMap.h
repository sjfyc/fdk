#ifndef __TILEMAP_H_INCLUDE__
#define __TILEMAP_H_INCLUDE__
#include "Types.h"
#include <fdkgame/NaviTileMap.h>

class TileMap
	: public fdk::Singleton<TileMap>
	, public fdkgame::navi::TileMap
{
	friend class fdk::Singleton<TileMap>;
	typedef fdkgame::navi::TileMap _Base;
public:		
	void draw();
	void drawCellInfo();
private:
	TileMap();
	~TileMap();
};

#define g_TileMap (TileMap::instance())
#endif
