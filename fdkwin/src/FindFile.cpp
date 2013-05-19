#include <fdkwin/FindFile.h>

namespace fdk { namespace win
{
	FDKWIN_API bool findFile(const CString& pattern, FindFileHandler& handler)
	{
		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(pattern.GetString(), &findData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		bool ret = false;
		do
		{
			if ((findData.cFileName[0] == _T('.') && findData.cFileName[1] == _T('\0')) ||
				(findData.cFileName[0] == _T('.') && findData.cFileName[1] == _T('.') && findData.cFileName[2] == _T('\0')) )
			{
				continue;
			}
			handler.handleFile(findData);
			ret = true;
		}
		while (FindNextFile(hFind, &findData));
		FindClose(hFind);
		return ret;
	}
}}
