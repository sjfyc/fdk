#ifndef __FDK_BASE_H_INCLUDE__
#define __FDK_BASE_H_INCLUDE__

/*
Build dll: Define FDK_DLL FDK_EXPORTS
Use dll: Define FDK_DLL
Build/Use StaticLib: Nothing need to be defined
*/
#ifdef FDK_DLL
	#ifdef FDK_EXPORTS
	#define FDK_API __declspec(dllexport)
	#else
	#define FDK_API __declspec(dllimport)
	#endif
#else
	#define FDK_API
#endif

#include <assert.h>
#include <memory.h>
#include <string>

//----------------------------------------------------------------
// Macros
//----------------------------------------------------------------
#define FDK_NULL 0

/*
DELETE, DELETE_ARRAY
*/
#define FDK_DELETE(p) do { delete (p); (p) = NULL; } while(0)
#define FDK_DELETE_ARRAY(array) do { delete [](array); (array) = NULL; } while(0)

/*
DIM
*/
namespace fdk
{
	template <int N>
	struct _ArraySizeStruct
	{
		char c[N];
	};
	template <class T, int N>
	_ArraySizeStruct<N> const& _testArraySize(T (&)[N]);
}

#define FDK_DIM(array) (sizeof(fdk::_testArraySize(array).c)/sizeof(char))

/*
ASSERT, STATIC_ASSERT
*/
#define FDK_ASSERT assert

namespace fdk
{
	template <bool> struct _StaticAssert
	{
		_StaticAssert(...) {};
	};
	template <> struct _StaticAssert<false> 
	{
	};
}

#define FDK_STATIC_ASSERT(expr) \
	{ \
		class _STATIC_ASSERT_FAIL {} _msg; \
		sizeof((fdk::_StaticAssert<expr>)(_msg)); \
	}

/*
CMP_PTR
*/
#define FDK_CMP_PTR(CLASS, NAME)\
	char __cmp_inner_buffer_##NAME[sizeof(CLASS)]; \
	CLASS* NAME = reinterpret_cast<CLASS*>(__cmp_inner_buffer_##NAME);

/*
AUTO_COMPARE：比较操作符的自动实现: 需要T实现==和<
*/
#define FDK_AUTO_COMPARE(T) \
	inline bool operator!=(const T& a, const T& b) \
	{ \
		return !(a == b); \
	} \
	inline bool operator>(const T& a, const T& b) \
	{ \
		return b < a; \
	} \
	inline bool operator<=(const T& a, const T& b) \
	{ \
		return !(b < a); \
	} \
	inline bool operator>=(const T& a, const T& b) \
	{ \
		return !(a < b); \
	}

#define FDK_AUTO_COMPARE_MEMBER(T) \
	bool operator!=(const T& b) const\
	{ \
		return !(*this == b); \
	} \
	bool operator>(const T& b) const \
	{ \
		return b < *this; \
	} \
	bool operator<=(const T& b) const \
	{ \
		return !(b < *this); \
	} \
	bool operator>=(const T& b) const \
	{ \
		return !(*this < b); \
	}

namespace fdk
{
	typedef unsigned char byte_t;
	typedef unsigned short word_t;
	typedef unsigned long dword_t;
	typedef unsigned long long qword_t;

	class Uncopyable
	{
	public:
		Uncopyable() {}
		~Uncopyable() {}
	private:
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	};

	template <class T>
	class Singleton
		: private Uncopyable
	{
	public:
		static T& instance() 
		{
			static T inst;
			return inst;
		}
	};
	
	template <class T>
	T& constCast(const T& value)
	{
		return const_cast<T&>(value);
	}

	template <class T>
	T* constCast(const T* value)
	{
		return const_cast<T*>(value);
	}

	const size_t STATIC_BUFFER_SIZE = 4096;
	inline char* getStaticBuffer()
	{
		static char buffer[STATIC_BUFFER_SIZE+1] = { '\0' };
		return buffer;
	}

	template <class T, int N>
	inline void zeroArray(T (&o)[N])
	{
		memset(&o, 0, sizeof(o));
	}

	template <class T>
	inline void zeroObject(T& o)
	{
		memset(&o, 0, sizeof(o));
	}

	template <class T>
	inline const T& minOf(const T& a, const T& b)
	{
		return (a < b) ? a : b;
	}

	template <class T>
	inline const T& maxOf(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}

	template <class T>
	inline const T& clamp(const T& value, const T& minValue, const T& maxValue)
	{
		FDK_ASSERT(minValue <= maxValue);
		return (value < minValue) ? minValue : ( (value > maxValue) ? maxValue : value );
	}
		
	FDK_API std::string format(const char* fmt, ...);
	FDK_API void format(std::string& output, const char* fmt, ...);
}

#endif
