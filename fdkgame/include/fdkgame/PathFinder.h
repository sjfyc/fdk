#ifndef __FDKGAME_PATHFINDER_H_INCLUDE__
#define __FDKGAME_PATHFINDER_H_INCLUDE__
#include <fdkgame/Base.h>
#include <vector>
namespace fdk { namespace game {
	//class _PathFinderTypes
	//{
	//public:
	//	typedef Size<short> CellSize;
	//	typedef Coord<short> CellCoord;
	//	typedef Rect<short> CellRange;		
	//	typedef Coord<int> Location;
	//	typedef std::vector<Location> Path;	
	//protected:
	//	
	//	typedef byte_t CellState;
	//	typedef dword_t CellIndex;	
	//	typedef dword_t FCostType;
	//	typedef dword_t GCostType;
	//	typedef word_t HCostType; // 用word_t以节省内存（见CellData）
	//	struct OpenCell
	//	{
	//		CellIndex index;
	//		FCostType fcost;
	//	};
	//	struct OpenCellCmp
	//	{
	//		bool operator()(const OpenCell& a, const OpenCell& b) const
	//		{
	//			return a.fcost > b.fcost;
	//		}
	//	};
	//	typedef std::priority_queue<OpenCell, std::vector<OpenCell>, OpenCellCmp> OpenList;
	//};

	//class PathFinder
	//	: public _PathFinderTypes
	//{		
	//public:		
	//	PathFinder(
	//		const CellRange& cellRange,
	//		const CellSize& cellSize,
	//		const Location& fromLocation,
	//		const Location& toLocation
	//		);
	//	void find(Path& path);
	//	virtual bool canMove(const CellCoord& coord) const = 0;
	//private:
	//	enum InitResult
	//	{
	//		InitResult_OK,
	//		InitResult_Fail,
	//		InitResult_Finish,
	//	};
	//	InitResult init();
	//	CellCoord locationToCellCoord(const Location& location);
	//	OpenList m_openList;
	//	const Location& m_fromLocation;
	//	const Location& m_toLocation;	
	//	const CellRange& m_cellRange;
	//	const CellSize& m_cellSize;
	//	CellCoord m_fromCell;
	//	CellCoord m_toCell;
	//	CellIndex m_fromIndex;
	//};
}}

#endif
