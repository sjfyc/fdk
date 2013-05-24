namespace Fdk
{
    /// <summary>
    /// 单例模板
    /// </summary>
    public class Singleton<T> where T : new()
    {
        /// <summary>
        /// 返回具体类型的实例
        /// </summary>
        public static T Instance
        {
            get
            {
                return s_instance;
            }
        }

        private static readonly T s_instance = new T();
    }
}
