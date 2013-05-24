using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;

namespace Fdk
{   
    /// <summary>
    /// 全局框架类
    /// </summary>
    public class Framework : Singleton<Framework>
    {
        /// <summary>
        /// 模块错误
        /// </summary>
        public class ModuleError : Exception
        {
            /// <summary>
            /// 模块名称
            /// </summary>
            public Module Module
            {
                get
                {
                    return m_module;
                }
            }

            /// <summary>
            /// 构造函数
            /// </summary>
            protected ModuleError(Module module)
            {
                m_module = module;
            }

            private Module m_module;
        }

        /// <summary>
        /// 模块启动错误
        /// </summary>
        public class ModuleStartError : ModuleError
        {
            internal ModuleStartError(Module module)
                : base(module)
            {
            }
        }

        /// <summary>
        /// 模块例程错误
        /// </summary>
        public class ModuleRoutineError : ModuleError
        {
            internal ModuleRoutineError(Module module)
                : base(module)
            {
            }
        }

        /// <summary>
        /// 模块循环依赖错误
        /// </summary>
        public class ModulesCycleDependError : Exception
        {
            /// <summary>
            /// 相互依赖的模块列表
            /// </summary>
            public ReadOnlyCollection<Module> Modules
            {
                get
                {
                    return m_modules.AsReadOnly();
                }
            }
 
            internal ModulesCycleDependError(List<Module> modules)
            {
                m_modules = modules;
            }

            private List<Module> m_modules;
        }        
               
        /// <summary>
        /// 框架是否已经启动
        /// </summary>
        public bool IsStarted
        {
            get
            {
                return m_started;
            }
        }

        /// <summary>
        /// 搭载的模块列表
        /// </summary>
        public ReadOnlyCollection<Module> Modules
        {
            get
            {
                return m_modules.AsReadOnly();
            }
        }

        /// <summary>
        /// <para>@require module != null</para>
        /// </summary>
        public bool ContainsModule(Module module)
        {
            Debug.Assert(module != null);
            return m_modules.Contains(module);
        }

        /// <summary>
        /// 注册模块：必须在框架启动前注册所有模块
        /// <para>@require module != null</para>
        /// <para>@require !ContainsModule(module)</para>
        /// <para>@require Debug.Assert(!module.IsStarted);</para>
        /// </summary>
        public void RegisterModule(Module module)
        {
            Debug.Assert(module != null);
            Debug.Assert(!ContainsModule(module));
            Debug.Assert(!module.IsStarted);
            m_modules.Add(module);
            Debug.Assert(ContainsModule(module));
        }

        /// <summary>
        /// 卸载模块
        /// <para>@require module != null</para>
        /// <para>@require ContainsModule(module)</para>
        /// </summary>
        public void UnregisterModule(Module module)
        {
            Debug.Assert(module != null);
            Debug.Assert(ContainsModule(module));
            bool result = m_modules.Remove(module);
            Debug.Assert(result);
            Debug.Assert(!ContainsModule(module));
        }

        /// <summary>
        /// 启动框架：按模块依赖顺序启动所有模块
        /// <para>@require !IsStarted</para>
        /// </summary>
        /// <exception cref="ModuleStartError">模块启动失败</exception>
        /// <exception cref="ModulesCycleDependError">模块循环依赖错误</exception>
        public void Start()
        {
            Debug.Assert(!IsStarted);

            List<Module> leftModules = new List<Module>();
            List<Module> pendingModules = new List<Module>();
            foreach (Module module in m_modules)
            {
                leftModules.Add(module);
            }

            while (true)
            {
                if (leftModules.Count <= 0)
                {
                    m_started = true;
                    return;
                }
                foreach (Module module in leftModules)
                {
                    if (!module.CheckStart())
                    {
                        pendingModules.Add(module);
                        continue;
                    }
                    if (!module.Start())
                    {
                        StopStartedModules();
                        throw new ModuleStartError(module);
                    }
                    m_startedModules.Add(module);
                    module.SetStarted(true);
                }
                if (pendingModules.Count == leftModules.Count)
                {
                    StopStartedModules();
                    throw new ModulesCycleDependError(leftModules);
                }
                leftModules = pendingModules;
                pendingModules = new List<Module>();
            }
        }

        /// <summary>
        /// 停止框架：按启动相反顺序停止所有模块
        /// <para>@require IsStarted</para>
        /// </summary>
        public void Stop()
        {
            Debug.Assert(IsStarted);
            StopStartedModules();
            m_started = false;
        }

        /// <summary>
        /// 框架例程：按启动顺序执行所有模块例程
        /// <para>@require IsStarted</para>
        /// </summary>
        /// <exception cref="ModuleRoutineError">模块运行异常</exception>
        public void Routine()
        {
            Debug.Assert(IsStarted);
            
            foreach (Module module in m_startedModules)
            {
                if (!module.Routine())
                {
                    throw new ModuleRoutineError(module);
                }
            }
        }

        private void StopStartedModules()
        {
            if (m_startedModules.Count <= 0)
            {
                return;
            }
            for (int i = m_startedModules.Count - 1; i >= 0; --i)
            {
                Module module = m_startedModules[i];
                module.Stop();
                module.SetStarted(false);
            }
            m_startedModules.Clear();
        }

        private List<Module> m_modules = new List<Module>();
        private List<Module> m_startedModules = new List<Module>();
        private bool m_started = false;
    }
}
