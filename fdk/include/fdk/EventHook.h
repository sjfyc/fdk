#ifndef __FDK_EVENTHOOK_H_INCLUDE__
#define __FDK_EVENTHOOK_H_INCLUDE__
#include "Base.h"
#include <set>
#include <map>

namespace fdk
{
	class FDK_API EventHook
	{
	public:
		virtual ~EventHook() {}
		virtual void onEvent(int eventType, void* params) {}
		void regist(int eventType);
		void unregist(int eventType);
		void unregist();
	protected:
		EventHook() {}
	};

	class FDK_API EventCenter
		: public fdk::Singleton<EventCenter>
	{
		typedef std::set<EventHook*> Hooks;
		typedef std::map<int, Hooks> RegisteredHooks;
	public:
		void regist(EventHook& hook, int eventType);
		void unregist(EventHook& hook, int eventType);
		void unregist(EventHook& hook);
		void send(int eventType, void* params);
	private:		
		RegisteredHooks m_hooks;
	};

	inline void EventHook::regist(int eventType)
	{
		EventCenter::instance().regist(*this, eventType);
	}

	inline void EventHook::unregist(int eventType)
	{
		EventCenter::instance().unregist(*this, eventType);
	}

	inline void EventHook::unregist()
	{
		EventCenter::instance().unregist(*this);
	}
}

#endif
