#ifndef __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#define __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class BlockMap;	

	// ��Ӧĳ�ֳߴ絥λ�Ķ����ͼ��ÿ������洢���ڸóߴ絥λ����ײ����
	class FDKGAME_API VertexMap
	{
	public:
		VertexMap(BlockMap& blockMap, UnitSize unitSize);
		~VertexMap();		
		void rebuildFromBlockMap();		
		void onSetBlock(const CellCoord& cellCoord, bool bSet);
		size_t getSizeX() const;
		size_t getSizeY() const;
		int getBlockValue(const CellCoord& cellCoord) const;
	private:
		BlockMap& m_blockMap;			
		UnitSize m_unitSize;
		Array2D<int> m_data;
	};
}}}

#endif
