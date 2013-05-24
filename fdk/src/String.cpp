#include <fdk/String.h>

namespace fdk
{
	namespace
	{
		inline bool isSpace(char c) // VC的isspace函数在debug中文时会assert
		{
			return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
		}
	}

	FDK_API void trimLeft(std::string& s)
	{
		if (s.empty())
		{
			return;
		}
		const char* c = s.c_str();
		if (!isSpace(*c))
		{
			return;
		}
		++c;
		while (isSpace(*c)) 
		{
			++c;
		}
		s = c;
	}

	FDK_API void trimRight(std::string& s)
	{
		if (s.empty())
		{
			return;
		}
		const char* begin = s.c_str();
		const char* end = begin+s.length()-1;
		while (end >= begin && isSpace(*end))
		{
			--end;
		}
		s = s.substr(0, end+1-begin);
	}

	FDK_API void trim(std::string& s)
	{
		if (s.empty())
		{
			return;
		}
		const char* begin = s.c_str();
		const char* end = begin+s.length()-1;
		while (isSpace(*begin))
		{
			++begin;
		}
		while (end > begin && isSpace(*end))
		{
			--end;
		}
		s = s.substr(begin-s.c_str(), end+1-begin);
	}

	FDK_API size_t replace(std::string& s, const std::string& from, const std::string& to)
	{
		std::string::size_type pos = 0;
		size_t found = 0;
		while (1)
		{
			pos = s.find(from, pos);
			if (pos == std::string::npos)
			{
				break;
			}
			s.replace(pos, from.length(), to);
			pos += from.length();
			++found;
		}
		return found;
	}
}
