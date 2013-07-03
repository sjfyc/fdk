#ifndef __BOARD_H_INCLUDE__
#define __BOARD_H_INCLUDE__
#include "Types.h"
#include <fdkgame/NaviGridMap.h>

class Board
	: public fdkgame::navi::GridMap
	, public fdk::Singleton<Board>
{
	friend class fdk::Singleton<Board>;
	typedef fdkgame::navi::GridMap _Base;
public:		
	bool isBlock(const CellCoord& coord) const;
	void setBlock(const CellCoord& coord, bool bBlock);
	void clearBlocks();
	void draw();
	void drawCellCoord();
	void drawCenterVertex();
	void drawCellClearanceValue();
	fdk::Array2D<int> m_areaID;
private:
	Board();
	~Board();
	bool isValidCoord(const CellCoord& coord) const;	
};

#define g_Board (Board::instance())
#endif
