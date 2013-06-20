extern void testMisc(const char* argv0);
extern void testString();
extern void testArray2D();
extern void testModule();
extern void testIterator();
extern void testRand();

int main(int argc, char* argv[])
{
	testMisc(argv[0]);
	testString();
	testArray2D();
	testModule();
	testIterator();
	testRand();
	return 0;
}
