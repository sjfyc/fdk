#ifndef __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#define __FDKGAME_NAVIVERTEXMAP_H_INCLUDE__
#include <fdk/Array2D.h>
#include "NaviTypes.h"

namespace fdk { namespace game { namespace navi
{
	class BlockMap;

	typedef Vector2D<int> VertexCoord;

	// ��Ӧĳ�ֳߴ絥λ�Ķ����ͼ��ÿ������洢���ڸóߴ絥λ����ײ����
	class FDKGAME_API VertexMap
	{
	public:
		VertexMap(int unitSize);
		~VertexMap();
		void rebuildFromBlockMap(const BlockMap& blockMap);
		void onSetBlock(const CellCoord& cellCoord, bool bSet);
	private:
		int m_unitSize;
		Array2D<int> m_data;
	};
}}}

#endif
