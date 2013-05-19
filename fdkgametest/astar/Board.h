#ifndef __BOARD_H_INCLUDE__
#define __BOARD_H_INCLUDE__
#include "Types.h"

class Board
	: public fdk::Singleton<Board>
{
public:
	Board();
	~Board();	
	bool isBlock(const CellCoord& coord) const;
	void setBlock(const CellCoord& coord, bool bBlock);
	void draw();
private:
	bool isValidCoord(const CellCoord& coord) const;
	bool m_block[CELL_COUNT_X][CELL_COUNT_Y];
};

#define g_Board (Board::instance())
#endif
