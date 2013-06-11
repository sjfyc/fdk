#include <fdk/Module.h>

namespace fdk
{
	void ModuleManager::regist(
		_Module& module,
		ModuleCheckStartFunc checkStartFunc,
		ModuleStartFunc startFunc,
		ModuleStopFunc stopFunc,
		ModuleTickFunc tickFunc
		)
	{
		ModuleInfo moduleInfo;
		moduleInfo.module = &module;
		moduleInfo.checkStartFunc = checkStartFunc;
		moduleInfo.startFunc = startFunc;
		moduleInfo.stopFunc = stopFunc;
		moduleInfo.tickFunc = tickFunc;
		m_modules.push_back(moduleInfo);
	}

	bool ModuleManager::start()
	{
		std::vector<ModuleInfo*> left;
		std::vector<ModuleInfo*> pending;
		for (size_t i = 0; i < m_modules.size(); ++i)
		{
			left.push_back(&m_modules[i]);
		}
		while (1)
		{
			for (size_t i = 0; i < left.size(); ++i)
			{
				ModuleInfo& moduleInfo = *left[i];
				_Module& module = *moduleInfo.module;
				if (moduleInfo.checkStartFunc && !(*moduleInfo.checkStartFunc)(module))
				{
					pending.push_back(&moduleInfo);
					continue;
				}
				if (moduleInfo.startFunc && !(*moduleInfo.startFunc)(module))
				{
					stopStartedModules();
					m_tickModules.clear();
					m_errorMessage = "failed to start module '";
					m_errorMessage += module.getName();
					m_errorMessage += "'";
					return false;
				}
				module.m_bStarted = true;
				m_startedModules.push_back(&moduleInfo);
				if (moduleInfo.tickFunc)
				{
					m_tickModules.push_back(&moduleInfo);
				}				
			}
			if (pending.empty())
			{
				return true;
			}
			if (pending.size() == left.size())
			{
				m_errorMessage = "circular dependencies of modules[ ";
				for (size_t i = 0; i < left.size(); ++i)
				{
					ModuleInfo& moduleInfo = *left[i];
					m_errorMessage += "'";
					m_errorMessage += moduleInfo.module->getName();
					m_errorMessage += "'";
					if (i != left.size()-1)
					{
						m_errorMessage += ", ";
					}
				}
				m_errorMessage += " ]";
				return false;
			}
			left.swap(pending);
			pending.clear();
		}
	}

	void ModuleManager::stop()
	{	
		stopStartedModules();
	}

	bool ModuleManager::tick(float deltaSeconds)
	{
		for (size_t i = 0; i < m_tickModules.size(); ++i)
		{
			ModuleInfo& moduleInfo = *m_tickModules[i];			
			if (!(*moduleInfo.tickFunc)(*moduleInfo.module, deltaSeconds))
			{
				m_errorMessage = "failed to tick module '";
				m_errorMessage += moduleInfo.module->getName();
				m_errorMessage += "'";
				return false;
			}
		}
		return true;
	}

	void ModuleManager::stopStartedModules()
	{		
		for (size_t i = m_startedModules.size(); i > 0; --i)
		{
			ModuleInfo& moduleInfo = *m_startedModules[i-1];
			_Module& module = *moduleInfo.module;
			if (moduleInfo.stopFunc)
			{
				(*moduleInfo.stopFunc)(module);
			}
			module.m_bStarted = false;
		}
		m_startedModules.clear();
	}
}
