#include <fdk/Module.h>

namespace TestModule
{
	class Module1
		: public fdk::Module<Module1>
	{
	public:
		virtual const char* getName() const { return "Module1"; }
	};

	class Module2
		: public fdk::Module<Module2>
	{

	};
}

void testModule()
{
	fdk::ModuleManager::instance().start();
	TestModule::Module1::instance().getName();
}
