#ifndef __LIB2_H
#define __LIB2_H
#include <iostream>

class Lib2
{
public:
	Lib2() : m_value(0) {}
	void create()
	{
		std::cout << "Lib2::create" << std::endl;
		m_value = new int(10);
	}
	void destroy()
	{		
		delete m_value;
		m_value = 0;
		std::cout << "Lib2::destroy" << std::endl;
	}
	int* newptr();
private:
	int* m_value;
};

#endif
