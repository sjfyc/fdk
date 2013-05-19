#include <fdkwin/Encode.h>

namespace fdk { namespace win
{
	namespace
	{
		void mb_to_wc(std::wstring& output, const std::string& src, DWORD codePage)
		{
			int wlen = ::MultiByteToWideChar(codePage, 0, src.c_str(), (int)src.length(), NULL, 0);
			wchar_t* wbuf = new wchar_t[wlen+1];
			::MultiByteToWideChar(codePage, 0, src.c_str(), (int)src.length(), wbuf, wlen);
			wbuf[wlen] = L'\0';
			output = wbuf;
			delete []wbuf;
		} 

		void wc_to_mb(std::string& output, const std::wstring& src, DWORD codePage)
		{
			int len = ::WideCharToMultiByte(codePage, 0, src.c_str(), (int)src.length(), NULL, 0, 0, 0); 
			char* buf = new char[len+1];
			::WideCharToMultiByte(codePage, 0, src.c_str(), (int)src.length(), buf, len, 0, 0); 
			buf[len] = '\0';
			output = buf;
			delete []buf;
		}
	}

	FDKWIN_API void ansi_to_unicode(std::wstring& output, const std::string& input)
	{
		mb_to_wc(output, input, CP_ACP);
	}

	FDKWIN_API void unicode_to_ansi(std::string& output, const std::wstring& input)
	{
		wc_to_mb(output, input, CP_ACP);
	}

	FDKWIN_API void utf8_to_unicode(std::wstring& output, const std::string& input)
	{
		mb_to_wc(output, input, CP_UTF8);
	}

	FDKWIN_API void unicode_to_utf8(std::string& output, const std::wstring& input)
	{
		wc_to_mb(output, input, CP_UTF8);
	}

	FDKWIN_API void ansi_to_utf8(std::string& output, const std::string& input)
	{
		std::wstring temp;
		ansi_to_unicode(temp, input);
		unicode_to_utf8(output, temp);
	}

	FDKWIN_API void utf8_to_ansi(std::string& output, const std::string& input)
	{
		std::wstring temp;
		utf8_to_unicode(temp, input);
		unicode_to_ansi(output, temp);
	}
}}
