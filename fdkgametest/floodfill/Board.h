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
	typedef fdkgame::navi::GridEnvColorComponent ColorComponent;
	bool isBlock(const CellCoord& coord) const;
	void setBlock(const CellCoord& coord, bool bBlock);
	void clearBlocks();
	void draw();
	void drawCellColor();
	void drawCellCoord();
	void drawCenterVertex();
	void drawCellClearanceValue();
	void drawConnectors();
private:
	Board();
	~Board();
	bool isValidCoord(const CellCoord& coord) const;	
};
#define g_Board (Board::instance())
#endif
