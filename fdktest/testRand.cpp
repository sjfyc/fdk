#include <fdk/Rand.h>
#include <iostream>
#include <time.h>
void testRand()
{
	std::cout << "[[testRand begin" << std::endl;
	srand((unsigned int)time(0));
	for (size_t i = 0; i < 1000; ++i)
	{
		std::cout << fdk::rand(-2, 5) << ",";
	}	
	std::cout << std::endl << "testRand end]]" << std::endl;
}
