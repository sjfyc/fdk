#ifndef __ABSTRACTMAP_H_INCLUDE__
#define __ABSTRACTMAP_H_INCLUDE__
#include "Types.h"
#include <fdk/Base.h>
#include <fdkgame/FindPathHpa.h>
class Board;

class AbstractMap
	: public fdkgame::findpath::AbstractGridMap
	, public fdk::Singleton<AbstractMap>
{
	friend class fdk::Singleton<AbstractMap>;
	typedef fdkgame::findpath::AbstractGridMap _Base;
public:	
	AbstractMap();
	void draw();
private:
};

#define g_AbstractMap (AbstractMap::instance())
#endif
