#ifndef __FDK_MODULE_H_INCLUDE__
#define __FDK_MODULE_H_INCLUDE__
#include "Base.h"
#include <vector>
#include <string>

namespace fdk
{
	template<typename T>
	struct _ModuleTraits
	{
	private:
		template<typename U, bool (U::*)()> struct has_checkStart;
		template<typename U> static TrueType& test_checkStart(has_checkStart<U, &U::start>*);
		template<typename U> static FalseType& test_checkStart(...);

		template<typename U, bool (U::*)()> struct has_start;
		template<typename U> static TrueType& test_start(has_start<U, &U::start>*);
		template<typename U> static FalseType& test_start(...);

		template<typename U, void (U::*)()> struct has_stop;
		template<typename U> static TrueType& test_stop(has_stop<U, &U::stop>*);
		template<typename U> static FalseType& test_stop(...);

		template<typename U, bool (U::*)(float)> struct has_tick;		
		template<typename U> static TrueType& test_tick(has_tick<U, &U::tick>*);
		template<typename U> static FalseType& test_tick(...);
	public:
		enum
		{
			HAS_CHECKSTART_METHOD = (sizeof(test_checkStart<T>(0)) == sizeof(TrueType)),
			HAS_START_METHOD = (sizeof(test_start<T>(0)) == sizeof(TrueType)),
			HAS_STOP_METHOD = (sizeof(test_stop<T>(0)) == sizeof(TrueType)),
			HAS_TICK_METHOD = (sizeof(test_tick<T>(0)) == sizeof(TrueType)),
		};
	};

	class _Module
	{
		friend class ModuleManager;
	public:
		bool isStarted() const;
		virtual const char* getName() const = 0;
	protected:
		_Module();
		~_Module();
	private:
		bool m_bStarted;
	};

	template <class T>
	class Module 
		: _Module
		, public Singleton<T>
	{	
	protected:		
		Module();
		~Module();
	private:		
		static bool checkStart(_Module& module);
		static bool start(_Module& module);
		static void stop(_Module& module);
		static bool tick(_Module& module, float deltaSeconds);
		static Module& s_instance;
	};

	class ModuleManager
		: public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;
	public:		
		bool start();
		void stop();
		bool tick(float deltaSeconds);
		const std::string& getErrorMessage() const;
	private:
		typedef bool (*ModuleCheckStartFunc)(_Module& module);
		typedef bool (*ModuleStartFunc)(_Module& module);
		typedef void (*ModuleStopFunc)(_Module& module);
		typedef bool (*ModuleTickFunc)(_Module& module, float deltaSeconds);
		struct ModuleInfo
		{
			_Module* module;
			ModuleCheckStartFunc checkStartFunc;
			ModuleStartFunc startFunc;
			ModuleStopFunc stopFunc;
			ModuleTickFunc tickFunc;		
		};
		void regist(
			_Module& module,
			ModuleCheckStartFunc checkStartFunc,
			ModuleStartFunc startFunc,
			ModuleStopFunc stopFunc,
			ModuleTickFunc tickFunc
			);
		void stopStartedModules();
		std::vector<ModuleInfo> m_modules;
		std::vector<ModuleInfo*> m_startedModules;
		std::vector<ModuleInfo*> m_tickModules;
		std::string m_errorMessage;
		bool m_bStarted;
	};

	inline _Module::_Module()
		: m_bStarted(false)
	{}

	inline _Module::~_Module()
	{}

	inline bool _Module::isStarted() const
	{
		return m_bStarted;
	}
	
	template <class T>
	Module<T>& Module<T>::s_instance = Module<T>::instance();

	template <class T>
	inline Module<T>::Module()
	{
		ModuleManager::regist(*this, 
			_ModuleTraits<T>::HAS_CHECKSTART_METHOD ? checkStart : 0,
			_ModuleTraits<T>::HAS_START_METHOD ? start : 0,
			_ModuleTraits<T>::HAS_STOP_METHOD ? stop : 0,
			_ModuleTraits<T>::HAS_TICK_METHOD ? tick : 0
			);
	}

	template <class T>
	inline Module<T>::~Module()
	{}

	template <class T>
	inline bool Module<T>::checkStart(_Module& module)
	{
		T& self = static_cast<T&>(module);
		return self.checkStart();
	}

	template <class T>
	inline bool Module<T>::start(_Module& module)
	{
		T& self = static_cast<T&>(module);
		return self.start();
	}

	template <class T>
	inline void Module<T>::stop(_Module& module)
	{
		T& self = static_cast<T&>(module);
		self.stop();
	}

	template <class T>
	inline bool Module<T>::tick(_Module& module, float deltaSeconds)
	{
		T& self = static_cast<T&>(module);
		return self.tick(deltaSeconds);
	}

	inline const std::string& ModuleManager::getErrorMessage() const
	{
		return m_errorMessage;
	}
}

#endif
