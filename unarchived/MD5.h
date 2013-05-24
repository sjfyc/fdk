#ifndef __FDK_MD5_H_INCLUDE__
#define __FDK_MD5_H_INCLUDE__
#include <fdk/Base.h>
#include <string>

namespace fdk
{	
	const int MD5_LENGTH = 16; // MD5码长度为16字节，128位

	class FDK_API MD5
	{
	public:
		static const MD5& EMPTY(); // 空串的MD5码
		MD5(); // as empty
		explicit MD5(byte_t code[MD5_LENGTH]);
		MD5(const void* buffer, size_t size);
		explicit MD5(const std::string& s);
		void reset(); // to empty
		void from(byte_t code[MD5_LENGTH]);
		void from(const void* buffer, size_t size);
		void from(const std::string& s);
		bool fromFile(const char* fileName); // 文件无法打开或者读取则返回false
		const byte_t* digest() const;
		std::string toString() const;
		bool operator==(const MD5& o) const;
		bool operator<(const MD5& o) const;
	private:
		byte_t m_code[MD5_LENGTH];
	};
	
	class FDK_API MD5Updator
	{
	public:
		MD5Updator();
		~MD5Updator(); // 自动close
		bool open(); // 已Open返回false
		bool update(const void* buffer, size_t size); // 未Open返回false
		bool close(MD5* output=NULL); // 未Open返回false
	private:
		void* m_context;
		bool m_open;
	};

	inline const MD5& MD5::EMPTY()
	{
		static MD5 instance;
		return instance;
	}

	inline MD5::MD5(byte_t code[MD5_LENGTH])
	{
		from(code);
	}

	inline MD5::MD5(const void* buffer, size_t size)
	{
		from(buffer, size);
	}

	inline MD5::MD5(const std::string& s)
	{
		from(s);
	}

	inline void MD5::from(const std::string& s)
	{
		from(s.c_str(), s.length());
	}

	inline const byte_t* MD5::digest() const
	{
		return m_code;
	}
}

#endif
