#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <vector>
#include <set>
#include <fdkgame/NaviAStar.h>
#include <fdkgame/NaviJpsPlus.h>
#include "Types.h"

class AStarRecorder
	: public fdkgame::navi::AStarRecorder
{
	typedef fdkgame::navi::AStarRecorder _Recorder;
public:
	virtual void onOpenNode(const fdkgame::navi::Environment& env, int nodeID, int parentNodeID, bool bReopen);
	virtual void onCloseNode(const fdkgame::navi::Environment& env, int nodeID);
	void render();
protected:
	AStarRecorder();
private:
	struct RecNodeInfo
	{
		CellCoord cellCoord;
		int parentNodeID;
		int openSerial;
		bool operator<(const RecNodeInfo& info) const { return cellCoord < info.cellCoord; }
	};
	typedef std::set<RecNodeInfo> OpenCells;
	typedef std::vector<RecNodeInfo> CloseCells;	
	OpenCells m_openCells;
	CloseCells m_closeCells;
	CellCoord m_currentClosedCell;
	int m_nextOpenSerial;
};

class AStar	
	: public AStarRecorder
{
	typedef AStarRecorder _Recorder;
public:
	typedef fdkgame::navi::AStar Navigator;
	AStar(int startNodeID, int targetNodeID);
	~AStar();
	Navigator::SearchResult search(int step=-1) { return m_navigator->search(step, this); }
	Navigator::SearchResult getSearchResult() const { return m_navigator->getSearchResult(); }
	void getPath(std::list<int>& output) const { m_navigator->getPath(output); }
	void render();
private:
	Navigator* m_navigator;
};

#endif