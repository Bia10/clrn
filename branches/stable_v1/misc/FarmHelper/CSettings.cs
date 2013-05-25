using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FarmHelper
{
    public class CSettings
    {
        [Serializable]
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct SettingsStruct
        {
            public int m_nLogLevel;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
            public char[] m_sLogPath;
        };

        //! Объект настроек 
        SettingsStruct m_Settings;

        //! Локер настроек
        object m_Locker;

        //! Контсруктор
        public CSettings()
        {
            m_Settings = new SettingsStruct();
            m_Locker = new object();
        }

        //! Устанавливаем настройки
        public void Set(SettingsStruct Settings)
        {
            lock (m_Locker)
                m_Settings = Settings;
        }

        //! Получаем текущие настройки
        public SettingsStruct Get()
        {
            lock (m_Locker)
                return m_Settings;
        }

        //! По дефолту
        public void Reset()
        {
        }
    }
}
