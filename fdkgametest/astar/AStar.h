#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <set>
#include <fdkgame/NaviAStar.h>
#include <fdkgame/NaviJps.h>
#include "Types.h"

class AStarRecorder
	: public fdkgame::navi::AStarRecorder
{
	typedef fdkgame::navi::AStarRecorder _Recorder;
public:
	virtual void onOpenNode(const fdkgame::navi::Environment& env, int nodeID, int parentNodeID, bool bReopen);
	virtual void onCloseNode(const fdkgame::navi::Environment& env, int nodeID);
	void render();
	typedef std::set<CellCoord> Cells;
	Cells m_openCells;
	Cells m_closeCells;
	CellCoord m_currentClosedCell;
};

class AStar	
	: public AStarRecorder
{
	typedef AStarRecorder _Recorder;
public:
	typedef fdkgame::navi::AStar Navigator;
	AStar(int startNodeID, int targetNodeID);
	~AStar();
	Navigator::SearchResult search(int step=-1) { return m_navigator->search(step); }
	Navigator::SearchResult getSearchResult() const { return m_navigator->getSearchResult(); }
	const std::vector<int>& getPath() const { return m_navigator->getPath(); }
	void render();
private:
	Navigator* m_navigator;
};

#endif