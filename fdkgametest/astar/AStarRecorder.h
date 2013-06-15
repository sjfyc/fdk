#ifndef __ASTARRECORDER_H
#define __ASTARRECORDER_H
#include <set>
#include <fdkgame/FindPathAStar.h>
#include "Types.h"

class AStarRecorder
	: public fdkgame::findpath::AStarRecorder
{
public:
	virtual void onOpenNode(int nodeID, int parentNodeID, bool bReopen);
	virtual void onCloseNode(int nodeID);
	void render();
private:
	typedef std::set<CellCoord> Cells;
	Cells m_openCells;
	Cells m_closeCells;
};


#endif