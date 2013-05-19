#include <fdk/Misc.h>
#include <string.h>

namespace fdk
{
	FDK_API const char* getFileName(const char* path)
	{
		FDK_ASSERT(path);
		const char* curPos = path + strlen(path);
		if (curPos == path)
		{
			return path;
		}
		do 
		{
			--curPos;
			if (*curPos == '\\' || *curPos == '/')
			{
				return curPos+1;
			}
		} while (curPos != path);
		return path;
	}
}
