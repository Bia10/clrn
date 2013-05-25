using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FarmHelper
{
    public class CData
    {
        [Serializable]
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct DataStruct
        {
            public int m_nPlayerLevel;
        };

        //! Объект данных 
        DataStruct m_Data;

        //! Локер данных
        object m_Locker;

        //! Контсруктор
        public CData()
        {
            m_Data = new DataStruct();
            m_Locker = new object();
        }

        //! Устанавливаем данные
        public void Set(DataStruct Data)
        {
            lock (m_Locker)
                m_Data = Data;
        }

        //! Получаем данные
        public DataStruct Get()
        {
            lock (m_Locker)
                return m_Data;
        }
    }
}
