using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace Fdk.Collections
{
    /// <summary>
    /// 链式字典
    /// </summary>
    /// <typeparam name="TKey">键</typeparam>
    /// <typeparam name="TValue">值</typeparam>
    public class LinkedDictionary<TKey, TValue> where TKey : IComparable
    {
        /// <summary>
        /// 返回容器内元素的数量
        /// </summary>
        public int Count
        {
            get
            {
                return m_dictionary.Count;
            }
        }

        /// <summary>       
        /// <para>@require key != null</para>
        /// </summary>
        public bool ContainsKey(TKey key)
        {
            Debug.Assert(key != null);
            return m_dictionary.ContainsKey(key);
        }

        /// <summary>       
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// </summary>
        public TValue GetValue(TKey key)
        {
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));
            return m_dictionary[key].value;
        }

        /// <summary>
        /// <para>@require !IsEmpty</para>
        /// </summary>
        public TKey FirstKey
        {
            get
            {
                Debug.Assert(!IsEmpty);
                return m_list.First.Value;
            }
        }

        /// <summary>
        /// <para>@require !IsEmpty</para>
        /// </summary>
        public TKey LastKey
        {
            get
            {
                Debug.Assert(!IsEmpty);
                return m_list.Last.Value;
            }
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// <para>@require key.CompareTo(FirstKey) == 0</para>
        /// </summary>
        public TKey PrevKey(TKey key)
        {
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));
            Debug.Assert(key.CompareTo(FirstKey) != 0);
            return m_dictionary[key].node.Previous.Value;
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// <para>@require key.CompareTo(LastKey) != 0</para>
        /// </summary>
        public TKey NextKey(TKey key)
        {
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));
            Debug.Assert(key.CompareTo(LastKey) != 0);
            return m_dictionary[key].node.Next.Value;
        }
                
        /// <summary>
        /// 返回容器是否为空
        /// </summary>
        public bool IsEmpty
        {
            get
            {
                return Count == 0;
            }
        }

        /// <summary>
        /// 返回键列表
        /// </summary>
        public IEnumerable<TKey> KeyList
        {
            get
            {
                return m_list;
            }
        }

        /// <summary>
        /// 返回键集合
        /// </summary>
        public IEnumerable<TKey> KeySet
        {
            get
            {
                return m_dictionary.Keys;
            }
        }

        /// <summary>
        /// <para>@require existKey != null</para>
        /// <para>@require ContainsKey(existKey)</para>
        /// <para>@require key != null</para>
        /// <para>@require !ContainsKey(key)</para>
        /// </summary>
        public void AddBefore(TKey existKey, TKey key, TValue value)
        {
            Debug.Assert(existKey != null);
            Debug.Assert(ContainsKey(existKey));
            Debug.Assert(key != null);
            Debug.Assert(!ContainsKey(key));

            Data newData = new Data();
            newData.node = m_list.AddBefore(m_dictionary[existKey].node, key);
            newData.value = value;
            m_dictionary.Add(key, newData);
        }

        /// <summary>
        /// <para>@require existKey != null</para>
        /// <para>@require ContainsKey(existKey)</para>
        /// <para>@require key != null</para>
        /// <para>@require !ContainsKey(key)</para>
        /// </summary>
        public void AddAfter(TKey existKey, TKey key, TValue value)
        {
            Debug.Assert(existKey != null);
            Debug.Assert(ContainsKey(existKey));
            Debug.Assert(key != null);
            Debug.Assert(!ContainsKey(key));

            Data newData = new Data();
            newData.node = m_list.AddAfter(m_dictionary[existKey].node, key);
            newData.value = value;
            m_dictionary.Add(key, newData);
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require !ContainsKey(key)</para>
        /// </summary>
        public void AddFirst(TKey key, TValue value)
        {
            Debug.Assert(key != null);
            Debug.Assert(!ContainsKey(key));

            Data newData = new Data();
            newData.node = m_list.AddFirst(key);
            newData.value = value;
            m_dictionary.Add(key, newData);
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require !ContainsKey(key)</para>
        /// </summary>
        public void AddLast(TKey key, TValue value)
        {
            Debug.Assert(key != null);
            Debug.Assert(!ContainsKey(key));

            Data newData = new Data();
            newData.node = m_list.AddLast(key);
            newData.value = value;
            m_dictionary.Add(key, newData);
        }

        /// <summary>
        /// <para>@require unmovedKey != null</para>
        /// <para>@require ContainsKey(unmovedKey)</para>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// <para>@require unmovedKey.CompareTo(FirstKey) == 0 || PrevKey(unmovedKey).CompareTo(key) != 0</para>
        /// </summary>
        public void MoveBefore(TKey unmovedKey, TKey key)
        {
            Debug.Assert(unmovedKey != null);
            Debug.Assert(ContainsKey(unmovedKey));
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));
            Debug.Assert(unmovedKey.CompareTo(FirstKey) == 0 || PrevKey(unmovedKey).CompareTo(key) != 0);

            m_list.Remove(m_dictionary[key].node);
            m_dictionary[key].node = m_list.AddBefore(m_dictionary[unmovedKey].node, key);
        }

        /// <summary>
        /// <para>@require unmovedKey != null</para>
        /// <para>@require ContainsKey(unmovedKey)</para>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// <para>@require unmovedKey.CompareTo(LastKey) == 0 || NextKey(unmovedKey).CompareTo(key) != 0</para>
        /// </summary>
        public void MoveAfter(TKey unmovedKey, TKey key)
        {
            Debug.Assert(unmovedKey != null);
            Debug.Assert(ContainsKey(unmovedKey));
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));
            Debug.Assert(unmovedKey.CompareTo(LastKey) == 0 || NextKey(unmovedKey).CompareTo(key) != 0);

            m_list.Remove(m_dictionary[key].node);
            m_dictionary[key].node = m_list.AddAfter(m_dictionary[unmovedKey].node, key);
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// </summary>
        public void Remove(TKey key)
        {
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));

            m_list.Remove(m_dictionary[key].node);
            bool result = m_dictionary.Remove(key);
            Debug.Assert(result);
        }

        /// <summary>
        /// 清空容器内所有元素
        /// </summary>
        public void Clear()
        {
            m_dictionary.Clear();
            m_list.Clear();
        }

        /// <summary>
        /// <para>@require key != null</para>
        /// <para>@require ContainsKey(key)</para>
        /// </summary>
        public void SetValue(TKey key, TValue value)
        {
            Debug.Assert(key != null);
            Debug.Assert(ContainsKey(key));

            Data data = m_dictionary[key];
            data.value = value;
        }

        private class Data
        {
            public LinkedListNode<TKey> node;
            public TValue value;
        }

        private SortedDictionary<TKey, Data> m_dictionary = new SortedDictionary<TKey, Data>();
        private LinkedList<TKey> m_list = new LinkedList<TKey>();
    }
}
