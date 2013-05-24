#include <fdkgame/PathFinder.h>
#include <queue>

namespace fdk { namespace game { namespace _pathfinder {	
	enum CellStateEnum
	{
		CellState_Unknown = 0,
		CellState_CannotMove,
		CellState_Open,
		CellState_Closed,
	};
	typedef byte_t ECellState;
	typedef dword_t CellIndex;
	typedef Size<char> ParentCellOffset;
	typedef dword_t FCost;
	typedef dword_t GCost;
	typedef word_t HCost; // 用word_t以节省内存（见CellData）	
	struct CellData
	{
		ParentCellOffset parentOffset;
		GCost gCost;
		HCost hCost;
		FCost fCost() const { return gCost + hCost; }
	};
	enum { MAX_MAP_CELLS = 2048 };
	enum { FINDER_BUFF_SIZE = MAX_MAP_CELLS * MAX_MAP_CELLS };
	struct SharedData
	{
		SharedData() 
			: cellDatas(new CellData[FINDER_BUFF_SIZE])
			, cellStates(new ECellState[FINDER_BUFF_SIZE])
		{}
		~SharedData() 
		{
			delete []cellDatas;
			delete []cellStates;
		}
		void reuse(size_t cellCount)
		{
			memset(cellStates, CellState_Unknown, sizeof(ECellState)*cellCount);
		}
		CellData* cellDatas;
		ECellState* cellStates;
	};
	struct OpenItem
	{
		CellIndex index;
		FCost fCost;
	};
	struct OpenItemCmp
	{
		bool operator()(const OpenItem& a, const OpenItem& b) const
		{
			return a.fCost > b.fCost;
		}
	};
	typedef std::priority_queue<OpenItem, std::vector<OpenItem>, OpenItemCmp> OpenList;

	class Finder
	{
	public:
		Finder(
			const CellSize& cellSize,
			const CellRange& window, 
			const Location& startLocation,
			const Location& targetLocation	
		);
		bool find();
	private:		
		enum EInitResult
		{
			InitResult_OK,
			InitResult_Fail,
			InitResult_Finish,
		};		
		EInitResult init();
		bool main();
		void inspect(
			const CellCoord& testCell, 
			const CellOffset& parentOffset,
			GCost gCost
			);
		CellCoord locationToCellCoord(const Location& location) const
		{
			return fdkgame::locationToCellCoord(location, m_cellSize);
		}
		static SharedData s_sd;
		OpenList m_openList;
		const CellSize m_cellSize;
		const CellRange m_window, 
		const Location m_startLocation;
		const CellCoord m_startCellCoord;
		const Location m_targetLocation;
		const CellCoord m_targetCellCoord;
	};
	SharedData Finder::s_sd;
	
	//inline CellIndex toCellIndex(const CellRange& range, const CellCoord& coord) 
	//{
	//	FDK_ASSERT(range.contain(coord));
	//	return (coord.y - range.topLeft.y) * range.width() + (coord.x - range.topLeft.x);
	//}	

	Finder::Finder(
		const CellSize& cellSize,
		const CellRange& window, 
		const Location& startLocation,
		const Location& targetLocation
		)
		: m_cellSize(cellSize)
		, m_window(window)
		, m_startLocation(startLocation)
		, m_startCellCoord(fdkgame::locationToCellCoord(m_startLocation, m_cellSize))
		, m_targetLocation(targetLocation)
		, m_targetCellCoord(fdkgame::locationToCellCoord(m_targetLocation, m_cellSize))
	{}

	bool Finder::find()
	{
		EInitResult result = init();
		if (result == InitResult_Fail)
		{
			return false;
		}
		if (result == InitResult_Finish)
		{
			return true;
		}
		if (!main())
		{
			return false;
		}
		//generatePath();
		return true;
	}

	Finder::EInitResult Finder::init()
	{
		if (!m_window.contain(m_startCellCoord))
		{
			return InitResult_Fail;
		}
		if (m_fromLocation == m_toLocation)
		{
			return InitResult_Finish;
		}
		m_toCell = locationToCellCoord(m_toLocation);
	}
}}}