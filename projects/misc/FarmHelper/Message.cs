using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FarmHelper
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct PipeMessage
    {
        public enum __eMessageType
        {
            eSystem = 0,					//!< Системные сообщения (коненкт/дисконнект, старт, стоп)
            eText = 1,						//!< Текстовые сообщения
            eData = 2,						//!< Данные
            eSettings = 3					//!< Настройки
        };

        public int nID;							                    //!< ID сообщения
        public int nResponceID;					                    //!< Ответ на сообщение ID (если == 0, то это запрос)
        public int nType;							                //!< тип сообщения
        public int nFrom;							                //!< ID процесса, которому отправляем
        public int nTo;							                    //!< ID этого процесса
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public char[] Message;					                    //!< Сообщение
        public CData.DataStruct Data;			            	    //!< Данные
        public CSettings.SettingsStruct Settings;	                //!< Настройки
    };

}
