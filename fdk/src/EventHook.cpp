#include <fdk/EventHook.h>

namespace fdk
{
	void EventCenter::regist(EventHook& hook, int eventType)
	{
		m_hooks[eventType].insert(&hook);
	}

	void EventCenter::unregist(EventHook& hook, int eventType)
	{
		RegisteredHooks::iterator it = m_hooks.find(eventType);
		if (it == m_hooks.end())
		{
			return;
		}
		it->second.erase(&hook);
	}

	void EventCenter::unregist(EventHook& hook)
	{
		for (RegisteredHooks::iterator it = m_hooks.begin(); it != m_hooks.end(); ++it)
		{
			it->second.erase(&hook);
		}
	}

	void EventCenter::send(int eventType, void* params)
	{
		RegisteredHooks::iterator it = m_hooks.find(eventType);
		if (it == m_hooks.end())
		{
			return;
		}
		Hooks& hooks = it->second;
		for (Hooks::iterator itHook = hooks.begin(); itHook != hooks.end(); ++itHook)
		{
			EventHook* hook = *itHook;
			hook->onEvent(eventType, params);
		}
	}
}
