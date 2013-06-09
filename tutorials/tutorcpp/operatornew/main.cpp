#include <iostream>
#include <stdexcept>
#include <operatornewdeplib/lib.h>
#pragma warning(disable:4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

// 1. 不管何种形式的new调用，最终都由编译器分解为两个动作：
//	a. 调用对应的operator new 
//	b. 调用目标对象的构造函数（如果目标是个对象）
// 2. 当a成功而b失败时（比如构造函数抛出异常），编译器将自动调用与a匹配的operator delete
// 3. 所谓匹配指的是operator new和operator delete参数上的匹配
// 4. 如果operator new有自定义参数，那么也称之为placement new，对应delete也叫placement delete（注意placement delete只在2中被调用）

inline void printDelimLine(const char* title)
{
	std::cout << "------------------------" << std::endl;	
	std::cout << title << std::endl;
	std::cout << "------------------------" << std::endl;
}

namespace testClassedNew
{
	class Widget
	{
	public:
		// 函数名不一定必须要是set_new_handler，这里只是为了和标准库保持一致
		static std::new_handler set_new_handler(std::new_handler p) throw();

		// new Widget将调用Widget::operator new
		static void* operator new(std::size_t size) throw(std::bad_alloc);
	private:
		static std::new_handler s_currentHandler;
	};
	std::new_handler Widget::s_currentHandler = 0;

	std::new_handler Widget::set_new_handler(std::new_handler p) throw()
	{
		std::new_handler oldHandler = s_currentHandler;
		s_currentHandler = p;
		return oldHandler;
	}

	void* Widget::operator new(std::size_t size) throw(std::bad_alloc)
	{
		std::new_handler oldHandler = std::set_new_handler(s_currentHandler);
		try
		{
			void* p = ::operator new(size);
			std::set_new_handler(oldHandler);
			return p;
		}
		catch (...)
		{
			std::set_new_handler(oldHandler);
			throw;
		}
	}

	void Widget_outMemory()
	{
		std::cout << "outMemory" << std::endl;
	}

	void test()
	{
		Widget::set_new_handler(Widget_outMemory);
		Widget* p = new Widget;
		std::string* ps = new std::string;
		Widget::set_new_handler(0);
		Widget* p2 = new Widget;
	}

	// Mixin风格的基类NewHandlerSupport: 见EffectiveC++第3版中文版p245
}

namespace testPlacementNew
{
	struct Widget
	{
		Widget() { std::cout << "Widget()" << std::endl; }
		~Widget() { std::cout << "~Widget()" << std::endl; }
	};

	void test()
	{
		char rawMemory[100];
		Widget* b = new(rawMemory) Widget; // auto call Test constructor by placement new
		b->~Widget(); // must call destructor manually
	}
}

namespace testNothrowNew // nothrow new也是一种placement new
{
	class Widget
	{
	};

	void test()
	{
		// 实际调用全局的void * __CRTDECL operator new(size_t count, const std::nothrow_t&)
		// 在全局operator new之后自动调用目标的构造函数
		// new(std::nothrow) Widget只保证内存分配不会抛出异常，但无法保证Widget构造函数不抛出异常
		Widget* p = new(std::nothrow) Widget; 
		if (!p)
		{
			std::cout << "may reach here" << std::endl;
		}
		Widget* p2 = new Widget; // 如果失败抛出异常std::bad_alloc
		if (!p2)
		{
			std::cout << "never reach here" << std::endl;
		}
	}
}

namespace testCustomPlacementNew
{
	struct Widget
	{
		Widget() { std::cout << "Widget()" << std::endl;}
		~Widget() { std::cout << "~Widget()" << std::endl; }

		static void* operator new(std::size_t size, const char* file, long line) throw(std::bad_alloc)
		{
			std::cout << "Widget::new at " << file << ":" << line << std::endl;
			return malloc(size);
		}

		static void operator delete(void* p) throw()
		{
			std::cout << "Widget::delete" << std::endl;
			free(p);
		}

		// 该函数只在对应opeartor new成功，但目标构造函数抛出异常时调用
		static void operator delete(void* p, const char* file, long line) throw()
		{
			std::cout << "Widget::rollback delete at " << file << ":" << line << std::endl;
			free(p);
		}
	};

	void test()
	{
		Widget* p = new(__FILE__, __LINE__) Widget;
		delete p;
	}
}

inline void* operator new(std::size_t size) throw(std::bad_alloc)
{
	std::cout << "my global operator new" << std::endl;
	return ::malloc(size);
}

inline void operator delete(void* p) throw()
{
	std::cout << "my global operator delete" << std::endl;
	return ::free(p);
}

int main(int argc, char *argv[])
{	
	printDelimLine("testClassedNew");
	testClassedNew::test();
	printDelimLine("testPlacementNew");
	testPlacementNew::test();
	printDelimLine("testNothrowNew");
	testNothrowNew::test();
	printDelimLine("testCustomPlacementNew");
	testCustomPlacementNew::test();
	printDelimLine("Lib test");
	Lib testLib;
	testLib.create();
	testLib.destroy();
	delete testLib.newptr();
	return 0;
}
