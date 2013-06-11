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
		template<typename U, bool (U::*)() const> struct has_checkStart;
		template<typename U> static char test_checkStart(has_checkStart<U, &U::checkStart>*);
		template<typename U> static int test_checkStart(...);

		template<typename U, bool (U::*)()> struct has_start;
		template<typename U> static char test_start(has_start<U, &U::start>*);
		template<typename U> static int test_start(...);

		template<typename U, void (U::*)()> struct has_stop;
		template<typename U> static char test_stop(has_stop<U, &U::stop>*);
		template<typename U> static int test_stop(...);

		template<typename U, bool (U::*)(float)> struct has_tick;		
		template<typename U> static char test_tick(has_tick<U, &U::tick>*);
		template<typename U> static int test_tick(...);
	public:
		enum
		{
			HAS_CHECKSTART_METHOD = (sizeof(test_checkStart<T>(0)) == sizeof(char)),
			HAS_START_METHOD = (sizeof(test_start<T>(0)) == sizeof(char)),
			HAS_STOP_METHOD = (sizeof(test_stop<T>(0)) == sizeof(char)),
			HAS_TICK_METHOD = (sizeof(test_tick<T>(0)) == sizeof(char)),
		};		
	};

	class FDK_API _Module
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
		: public _Module
		, private Uncopyable
	{	
	public:
		static T& instance();
		virtual const char* getName() const;
		static Module& _s_instance;
	protected:
		Module();
		~Module();
	private:		
		static bool s_checkStart(const _Module& module);
		static bool s_start(_Module& module);
		static void s_stop(_Module& module);
		static bool s_tick(_Module& module, float deltaSeconds);
		static bool _checkStart(const T& module, TrueType);
		static bool _checkStart(const T& module, FalseType);
		static bool _start(T& module, TrueType);
		static bool _start(T& module, FalseType);
		static void _stop(T& module, TrueType);
		static void _stop(T& module, FalseType);
		static bool _tick(T& module, float deltaSeconds, TrueType);
		static bool _tick(T& module, float deltaSeconds, FalseType);		
	};

	class FDK_API ModuleManager
		: public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;
		template <class T> friend class Module;		
	public:		
		bool start();
		void stop();
		bool tick(float deltaSeconds);
		const std::string& getErrorMessage() const;
	private:
		typedef bool (*ModuleCheckStartFunc)(const _Module& module);
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
	inline T& Module<T>::instance() 
	{
		_s_instance;
		static T inst;
		return inst;
	}

	template <class T>
	Module<T>& Module<T>::_s_instance = T::instance();

	template <class T>
	const char* Module<T>::getName() const 
	{ 
		return T::NAME(); 
	}

	template <class T>
	inline Module<T>::Module()
	{
		ModuleManager::instance().regist(*this, 
			_ModuleTraits<T>::HAS_CHECKSTART_METHOD ? s_checkStart : 0,
			_ModuleTraits<T>::HAS_START_METHOD ? s_start : 0,
			_ModuleTraits<T>::HAS_STOP_METHOD ? s_stop : 0,
			_ModuleTraits<T>::HAS_TICK_METHOD ? s_tick : 0
			);
	}

	template <class T>
	inline Module<T>::~Module()
	{}

	template <class T>
	inline bool Module<T>::s_checkStart(const _Module& module)
	{
		const T& self = static_cast<const T&>(module);
		return _checkStart(self, typename BoolToTrueFalseType<_ModuleTraits<T>::HAS_CHECKSTART_METHOD>::Type());
	}

	template <class T>
	inline bool Module<T>::s_start(_Module& module)
	{
		T& self = static_cast<T&>(module);
		return _start(self, typename BoolToTrueFalseType<_ModuleTraits<T>::HAS_START_METHOD>::Type());	
	}

	template <class T>
	inline void Module<T>::s_stop(_Module& module)
	{
		T& self = static_cast<T&>(module);
		_stop(self, typename BoolToTrueFalseType<_ModuleTraits<T>::HAS_STOP_METHOD>::Type());
	}

	template <class T>
	inline bool Module<T>::s_tick(_Module& module, float deltaSeconds)
	{
		T& self = static_cast<T&>(module);
		return _tick(self, deltaSeconds, typename BoolToTrueFalseType<_ModuleTraits<T>::HAS_TICK_METHOD>::Type());
	}

	template <class T>
	inline bool Module<T>::_checkStart(const T& module, TrueType)
	{
		return module.checkStart();
	}

	template <class T>
	inline bool Module<T>::_checkStart(const T& module, FalseType)
	{
		FDK_ASSERT(0); // NEVER REACH HERE
		return false;
	}
	
	template <class T>
	inline bool Module<T>::_start(T& module, TrueType)
	{
		return module.start();
	}

	template <class T>
	inline bool Module<T>::_start(T& module, FalseType)
	{
		FDK_ASSERT(0); // NEVER REACH HERE
		return false;
	}

	template <class T>
	inline void Module<T>::_stop(T& module, TrueType)
	{
		module.stop();
	}

	template <class T>
	inline void Module<T>::_stop(T& module, FalseType)
	{}

	template <class T>
	inline bool Module<T>::_tick(T& module, float deltaSeconds, TrueType)
	{
		return module.tick(deltaSeconds);
	}
	
	template <class T>
	inline bool Module<T>::_tick(T& module, float deltaSeconds, FalseType)
	{
		FDK_ASSERT(0); // NEVER REACH HERE
		return false;
	}
	
	inline const std::string& ModuleManager::getErrorMessage() const
	{
		return m_errorMessage;
	}
}

#define FDK_MODULE_IMPL(T) \
	class _RegModule##T \
	{ \
	public:	\
		~_RegModule##T() \
		{ \
			T::_s_instance; \
		} \
	} _regModule##T;

#endif
