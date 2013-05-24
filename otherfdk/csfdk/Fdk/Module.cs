using System;

namespace Fdk
{
    /// <summary>
    /// 抽象模块类
    /// </summary>
    public abstract class Module
    {
        /// <summary>
        /// 名称
        /// </summary>
        public virtual string Name
        {
            get
            {
                return GetType().ToString();
            }
        }

        /// <summary>
        /// 是否已经启动
        /// </summary>
        public bool IsStarted
        {
            get
            {
                return m_started;
            }
        }

        /// <summary>
        /// 启动依赖性判定
        /// </summary>
        public virtual bool CheckStart() { return true; }

        /// <summary>
        /// 启动
        /// </summary>
        public abstract bool Start();
        
        /// <summary>
        /// 停止
        /// </summary>
        public abstract void Stop();
        
        /// <summary>
        /// 例程
        /// </summary>
        public abstract bool Routine();

        /// <summary>
        /// 构造函数
        /// </summary>
        protected Module()
        {
            m_started = false;
        }

        internal void SetStarted(bool value)
        {
            m_started = value;
        }

        private bool m_started;
    }
}
