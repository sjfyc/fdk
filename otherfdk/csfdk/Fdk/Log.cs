using System;
using System.IO;

namespace Fdk
{
    /// <summary>
    /// 日志等级
    /// </summary>
    public enum LogLevel
    {
        /// <summary>
        /// 全部关闭
        /// </summary>
        Off,
        /// <summary>
        /// 致命
        /// </summary>
        Fatal,
        /// <summary>
        /// 错误
        /// </summary>
        Error,
        /// <summary>
        /// 警告
        /// </summary>
        Warning,
        /// <summary>
        /// 信息
        /// </summary>
        Info,
        /// <summary>
        /// 全部关闭
        /// </summary>
        Debug,
        /// <summary>
        /// 全部开启
        /// </summary>
        All,
    }

    /// <summary>
    /// 全局日志类
    /// </summary>
    public class Log : Singleton<Log>
    {
        /// <summary>
        /// 日志等级
        /// </summary>
        public LogLevel Level
        {
            get
            {
                return m_level;
            }
            set
            {
                m_level = value;
            }
        }
        
        /// <summary>
        /// 是否写入控制台
        /// </summary>
        public bool IsWriteConsole 
        {
            get
            {
                return m_isWriteConsole;
            }
            set
            {
                m_isWriteConsole = value;
            }
        }

        /// <summary>
        /// 日志文件名称前缀
        /// </summary>
        public string FileNamePrefix
        {
            get
            {
                return m_fileNamePrefix;
            }
            set
            {
                m_fileNamePrefix = value;
            }
        }
        
        /// <summary>
        /// 是否启用致命日志
        /// </summary>
        public bool IsEnableFatal
        {
            get
            {
                return Level >= LogLevel.Fatal;
            }
        }

        /// <summary>
        /// 是否启用错误日志
        /// </summary>
        public bool IsEnableError
        {
            get
            {
                return Level >= LogLevel.Error;
            }
        }

        /// <summary>
        /// 是否启用警告日志
        /// </summary>
        public bool IsEnableWarning
        {
            get
            {
                return Level >= LogLevel.Warning;
            }
        }

        /// <summary>
        /// 是否启用信息日志
        /// </summary>
        public bool IsEnableInfo
        {
            get
            {
                return Level >= LogLevel.Info;
            }
        }

        /// <summary>
        /// 是否启用调试日志
        /// </summary>
        public bool IsEnableDebug
        {
            get
            {
                return Level >= LogLevel.Debug;
            }
        }

        /// <summary>
        /// 写入致命日志
        /// </summary>
        public void WriteFatal(string format, params object[] arg)
        {
            WriteLine(LogLevel.Fatal, format, arg);
        }

        /// <summary>
        /// 写入错误日志
        /// </summary>
        public void WriteError(string format, params object[] arg)
        {
            WriteLine(LogLevel.Error, format, arg);
        }

        /// <summary>
        /// 写入警告日志
        /// </summary>
        public void WriteWarning(string format, params object[] arg)
        {
            WriteLine(LogLevel.Warning, format, arg);
        }

        /// <summary>
        /// 写入信息日志
        /// </summary>
        public void WriteInfo(string format, params object[] arg)
        {
            WriteLine(LogLevel.Info, format, arg);
        }

        /// <summary>
        /// 写入调试日志
        /// </summary>
        public void WriteDebug(string format, params object[] arg)
        {
            WriteLine(LogLevel.Debug, format, arg);
        }

        private string MakeContentString(LogLevel level, string format, params object[] arg)
        {
            return string.Format("[{0} {1}] ", level.ToString(), DateTime.Now.ToString("HH:mm:ss"))
                + string.Format(format, arg);
        }

        private void WriteLine(LogLevel level, string format, params object[] arg)
        {
            if (m_level < level)
            {
                return;
            }
            string s = MakeContentString(level, format, arg);
            TryWriteConsole(s);
            WriteFiles(level, s);
        }

        private void TryWriteConsole(string content)
        {
            if (m_isWriteConsole)
            {
                Console.WriteLine(content);
            }
        }

        private void WriteFiles(LogLevel level, string content)
        {
            for (LogLevel i = level; i <= m_level; ++i)
            {
                WriteFile(i, content);
            }
        }

        private void WriteFile(LogLevel level, string content)
        {
            try
            {
                StreamWriter sw = new StreamWriter(GetFileName(level), true);
                sw.WriteLine(content);
                sw.Flush();
                sw.Close();
            }
            catch (IOException e)
            {
                Console.WriteLine("{0}", e.ToString());
            }
        }

        private string GetFileName(LogLevel level)
        {
            return m_fileNamePrefix + level.ToString() + DateTime.Now.ToString("yyyyMMdd") + ".log";
        }

        private LogLevel m_level = LogLevel.All;
        private bool m_isWriteConsole = true;
        private string m_fileNamePrefix = "Application";        
    }
}
