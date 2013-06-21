#ifndef __ABSTRACTMAP_H_INCLUDE__
#define __ABSTRACTMAP_H_INCLUDE__
#include "Types.h"
#include <fdk/Base.h>
#include <fdkgame/FindPathHpa.h>
class Board;
class Actor;

class HpaMap
	: public fdkgame::findpath::HpaMap
	, public fdk::Singleton<HpaMap>
{
	friend class fdk::Singleton<HpaMap>;
	typedef fdkgame::findpath::HpaMap _Base;
public:	
	HpaMap();
	void draw();
	bool reloadFromFile(const char* fileName);
private:
};

class Hpa
	: public fdkgame::findpath::Hpa
	, public fdkgame::findpath::EnvironmentChecker
{
	typedef fdkgame::findpath::Hpa _Base;
public:
	Hpa(Actor* initiator, fdkgame::findpath::HpaMap& env, int startNodeID, int targetNodeID);
	~Hpa();	
	void render();
	int popNextPathNode();

	// EnvironmentChecker interfaces
	virtual void onSearchBegining(const fdkgame::findpath::HpaMap& env, int startNodeID, int endNodeID);
	virtual void onSearchEnded(const fdkgame::findpath::HpaMap& env, int startNodeID, int endNodeID);
	virtual bool checkSuccessorNode(const fdkgame::findpath::HpaMap& env, int nodeID, int parentNodeID) const;

	virtual void onSearchBegining(const fdkgame::findpath::HpaCluster& env, int startNodeID, int endNodeID);
	virtual void onSearchEnded(const fdkgame::findpath::HpaCluster& env, int startNodeID, int endNodeID);
	virtual bool checkSuccessorNode(const fdkgame::findpath::HpaCluster& env, int nodeID, int parentNodeID) const;

	virtual void onSearchBegining(const fdkgame::findpath::GridMap& env, int startNodeID, int endNodeID);
	virtual void onSearchEnded(const fdkgame::findpath::GridMap& env, int startNodeID, int endNodeID);
	virtual bool checkSuccessorNode(const fdkgame::findpath::GridMap& env, int nodeID, int parentNodeID) const;
private:
	void ignoreAroundActors(const fdkgame::findpath::GridMap& env, const Location& center, float radius);
	std::vector<int> m_roughPath;
	std::vector<int> m_path;
	std::set<int> m_ignoredNodes;
	Actor* m_actor;
};

#define g_HpaMap (HpaMap::instance())
#endif
