extern void testMisc(const char* argv0);
extern void testString();

int main(int argc, char* argv[])
{
	testMisc(argv[0]);
	testString();
	return 0;
}
