#ifndef __BOARD_H_INCLUDE__
#define __BOARD_H_INCLUDE__
#include "Types.h"
#include <fdkgame/FindPathGridMap.h>

class Board
	: public fdkgame::findpath::GridMap
	, public fdk::Singleton<Board>
{
	friend class fdk::Singleton<Board>;
	typedef fdkgame::findpath::GridMap _Base;
public:		
	bool isBlock(const CellCoord& coord) const;
	void setBlock(const CellCoord& coord, bool bBlock);
	void draw();
private:
	Board();
	~Board();
	bool isValidCoord(const CellCoord& coord) const;
};

#define g_Board (Board::instance())
#endif
