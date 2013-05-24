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

	FDK_API std::string toHexString(const void* buffer, size_t size, bool bLower)
	{
		const byte_t* byteBuffer = static_cast<const byte_t*>(buffer);

		std::string s;
		for (size_t i = 0; i < size; ++i)
		{
			s += (bLower ? lowerHexOf(byteBuffer[i]) : upperHexOf(byteBuffer[i]));
		}
		return s;
	}
}
