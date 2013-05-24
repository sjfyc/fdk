#ifndef __FDK_STRING_H_INCLUDE__
#define __FDK_STRING_H_INCLUDE__
#include "Base.h"

namespace fdk
{
	FDK_API void trimLeft(std::string& s);
	FDK_API void trimRight(std::string& s);
	FDK_API void trim(std::string& s);
	FDK_API size_t replace(std::string& s, const std::string& from, const std::string& to);

	template <class T>
	bool toType(T& output, const std::string & s)
	{
		std::stringstream ss;
		ss << s;
		if (!(ss >> output))
		{
			return false;
		}
		char c;
		return !ss.get(c);
	};

	template <class T>
	inline bool isType(const std::string & s)
	{
		return toType(T(), s);
	};
}

#endif
