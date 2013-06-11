#include <fdk/Module.h>
#include <iostream>

namespace TestModule
{
	class Module1
		: public fdk::Module<Module1>
	{
	public:
		static const char* NAME() { return "Module1";  }
		bool start() { std::cout << "Module1::start" << std::endl; return true; }
		bool tick(float) { std::cout << "Module1::tick" << std::endl; return true; }
		void stop() { std::cout << "Module1::stop" << std::endl; }
		bool checkStart() const;
	};
	FDK_MODULE_IMPL(Module1)

	class Module2
		: public fdk::Module<Module2>
	{
	public:
		static const char* NAME() { return "Module2";  }
		bool start() { std::cout << "Module2::start" << std::endl; return true; }
		bool tick(float) { std::cout << "Module2::tick" << std::endl; return false; }
		void stop() { std::cout << "Module2::stop" << std::endl; }
		bool checkStart() const;
	};
	FDK_MODULE_IMPL(Module2)

	bool Module1::checkStart() const { return Module2::instance().isStarted(); }
	bool Module2::checkStart() const { return true; return Module1::instance().isStarted(); }
}

void testModule()
{
	fdk::ModuleManager& mgr = fdk::ModuleManager::instance();
	if (!mgr.start())
	{
		std::cout << mgr.getErrorMessage() << std::endl;
		return;
	}
	if (!mgr.tick(0))
	{
		std::cout << mgr.getErrorMessage() << std::endl;
	}
	mgr.stop();
}
