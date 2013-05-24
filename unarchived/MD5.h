#ifndef __FDK_MD5_H_INCLUDE__
#define __FDK_MD5_H_INCLUDE__
#include <fdk/Base.h>
#include <string>

namespace fdk
{	
	const int MD5_LENGTH = 16; // MD5�볤��Ϊ16�ֽڣ�128λ

	class FDK_API MD5
	{
	public:
		static const MD5& EMPTY(); // �մ���MD5��
		MD5(); // as empty
		explicit MD5(byte_t code[MD5_LENGTH]);
		MD5(const void* buffer, size_t size);
		explicit MD5(const std::string& s);
		void reset(); // to empty
		void from(byte_t code[MD5_LENGTH]);
		void from(const void* buffer, size_t size);
		void from(const std::string& s);
		bool fromFile(const char* fileName); // �ļ��޷��򿪻��߶�ȡ�򷵻�false
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
		~MD5Updator(); // �Զ�close
		bool open(); // ��Open����false
		bool update(const void* buffer, size_t size); // δOpen����false
		bool close(MD5* output=NULL); // δOpen����false
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
