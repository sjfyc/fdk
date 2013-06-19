#ifndef __ABSTRACTMAP_H_INCLUDE__
#define __ABSTRACTMAP_H_INCLUDE__
#include "Types.h"
#include <fdk/Base.h>
#include <fdkgame/FindPathHpa.h>
class Board;

class HpaMap
	: public fdkgame::findpath::HpaMap
	, public fdk::Singleton<HpaMap>
{
	friend class fdk::Singleton<HpaMap>;
	typedef fdkgame::findpath::HpaMap _Base;
public:	
	HpaMap();
	void draw();
private:
};

class Hpa
	: public fdkgame::findpath::Hpa
{
	typedef fdkgame::findpath::Hpa _Base;
public:
	Hpa(fdkgame::findpath::HpaMap& env, int startNodeID, int targetNodeID);
	~Hpa();	
	void render();
	SearchResult search();
	int popNextPathNode();
private:
	std::vector<int> m_abstractPath;
	std::vector<int> m_path;
};

#define g_HpaMap (HpaMap::instance())
#endif
