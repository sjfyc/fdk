#include <fdk/Iterator.h>
#include <vector>

void testIterator()
{
	typedef std::vector<int*> IV;
	typedef fdk::PointerIteratorToValueIterator<IV::iterator> iterator;
	typedef fdk::PointerIteratorToConstValueIterator<IV::iterator> const_iterator;

	IV iv;
	int a1=1, a2=2, a3=3;
	iv.push_back(&a1);
	iv.push_back(&a2);
	iv.push_back(&a3);

	for (iterator it = iv.begin(); it != iv.end(); ++it)
	{
		*it = 3;
	}

	for (const_iterator it = iv.begin(); it != iv.end(); ++it)
	{
		int i = *it;
	}
}
