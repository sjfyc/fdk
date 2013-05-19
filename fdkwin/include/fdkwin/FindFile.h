#ifndef __FDKWIN_FINDFILE_H_INCLUDE__
#define __FDKWIN_FINDFILE_H_INCLUDE__
#include "Base.h"

namespace fdk { namespace win
{
	class FDKWIN_API FindFileHandler
	{
	public:		
		virtual void handleFile(const WIN32_FIND_DATA& findData) = 0;
	};
	// �����.��..���ļ�(��)���������������ļ�(��)�򷵻�true
	FDKWIN_API bool findFile(const CString& pattern, FindFileHandler& handler);	

	inline bool isDirectory(const WIN32_FIND_DATA& findData)
	{
		return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
	}
}}

#endif
