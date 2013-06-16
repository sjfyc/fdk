#ifndef __ASTAR_H_INCLUDE__
#define __ASTAR_H_INCLUDE__
#include <set>
#include <fdkgame/FindPathAStar.h>
#include "Types.h"

class AStar
	: public fdkgame::findpath::AStarRecorder 
	, public fdkgame::findpath::AStar
{
	typedef fdkgame::findpath::AStar _Base;
	typedef fdkgame::findpath::AStarRecorder _Recorder;
public:
	AStar(const fdkgame::findpath::Environment& env, int startNodeID, int targetNodeID);
	~AStar();
	bool isEnded() const;
	// AStarRecorder interface
	virtual void onOpenNode(int nodeID, int parentNodeID, bool bReopen);
	virtual void onCloseNode(int nodeID);
	void render();
private:
	typedef std::set<CellCoord> Cells;
	Cells m_openCells;
	Cells m_closeCells;
	CellCoord m_currentClosedCell;
};

#endif