#include <iostream>
#include <stdexcept>
#include <operatornewdeplib/lib.h>
#pragma warning(disable:4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

// 1. ���ܺ�����ʽ��new���ã����ն��ɱ������ֽ�Ϊ����������
//	a. ���ö�Ӧ��operator new 
//	b. ����Ŀ�����Ĺ��캯�������Ŀ���Ǹ�����
// 2. ��a�ɹ���bʧ��ʱ�����繹�캯���׳��쳣�������������Զ�������aƥ���operator delete
// 3. ��νƥ��ָ����operator new��operator delete�����ϵ�ƥ��
// 4. ���operator new���Զ����������ôҲ��֮Ϊplacement new����ӦdeleteҲ��placement delete��ע��placement deleteֻ��2�б����ã�

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
		// ��������һ������Ҫ��set_new_handler������ֻ��Ϊ�˺ͱ�׼�Ᵽ��һ��
		static std::new_handler set_new_handler(std::new_handler p) throw();

		// new Widget������Widget::operator new
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

	// Mixin���Ļ���NewHandlerSupport: ��EffectiveC++��3�����İ�p245
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

namespace testNothrowNew // nothrow newҲ��һ��placement new
{
	class Widget
	{
	};

	void test()
	{
		// ʵ�ʵ���ȫ�ֵ�void * __CRTDECL operator new(size_t count, const std::nothrow_t&)
		// ��ȫ��operator new֮���Զ�����Ŀ��Ĺ��캯��
		// new(std::nothrow) Widgetֻ��֤�ڴ���䲻���׳��쳣�����޷���֤Widget���캯�����׳��쳣
		Widget* p = new(std::nothrow) Widget; 
		if (!p)
		{
			std::cout << "may reach here" << std::endl;
		}
		Widget* p2 = new Widget; // ���ʧ���׳��쳣std::bad_alloc
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

		// �ú���ֻ�ڶ�Ӧopeartor new�ɹ�����Ŀ�깹�캯���׳��쳣ʱ����
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
