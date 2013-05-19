#include <fdk/Base.h>
#include <stdarg.h>
#include <stdio.h>

namespace fdk
{
	FDK_API std::string format(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
#pragma warning(disable: 4996)
		vsnprintf(getStaticBuffer(), STATIC_BUFFER_SIZE, fmt, ap);
#pragma warning(default: 4996)
		va_end(ap);
		return getStaticBuffer();
	}

	FDK_API void format(std::string& output, const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
#pragma warning(disable: 4996)
		vsnprintf(getStaticBuffer(), STATIC_BUFFER_SIZE, fmt, ap);
#pragma warning(default: 4996)
		va_end(ap);
		output = getStaticBuffer();
	}
}
