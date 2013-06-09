#ifndef __LIB_H
#define __LIB_H
#include <iostream>

class Lib
{
public:
	Lib() : m_value(0) {}
	void create()
	{
		std::cout << "Lib::create" << std::endl;
		m_value = new int(10);
	}
	void destroy()
	{		
		delete m_value;
		m_value = 0;
		std::cout << "Lib::destroy" << std::endl;
	}
	int* newptr();
private:
	int* m_value;
};

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

#endif
