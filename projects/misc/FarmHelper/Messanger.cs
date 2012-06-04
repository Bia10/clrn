using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32.SafeHandles;
using System.Threading;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace FarmHelper
{
    //! Класс выполняющий все операции с достовкой/приемом сообщений
    public class CMessanger
    {
        //! Конструктор
        public CMessanger(int nServerID)
        {
            m_Data = new CData();
            m_Settings = new CSettings();
            m_nServerID = nServerID;
            m_nBufSize = 1024;
        }

	    //! Данные из сервера
	    CData m_Data;

	    //! Настройки
	    CSettings m_Settings;

	    //! ID сервера
        int m_nServerID;

	    //! Размер буфера
        int m_nBufSize;

        //! Стрим на чтение/запись
        FileStream m_Stream;

        //! Пытаться коннектится, вернет true если сервер создан
        bool IsServerCreated()
        {
            string sPipeName = "\\\\.\\pipe\\RECIEVE" + m_nServerID.ToString();
            bool bResult = Win32.WaitNamedPipe(sPipeName, 1000);
            return bResult;
        }

        //! Отправить сообщение
        bool Send(PipeMessage Message)
        {
            byte[] TempBuffer = MessageToByteArray(Message);
            m_Stream.Write(TempBuffer, 0, TempBuffer.Length);
            m_Stream.Flush();
            return true;
        }

        //! Генерация ответов
        void Generator(PipeMessage Message)
        {
            // есди это запрос то
            // отправляем "Ок" - сообщение получено
            if (Message.nResponceID != 0)
                return;
            PipeMessage Temp = Message;
            Temp.nResponceID = Temp.nID;
            Temp.nID = DateTime.Now.Millisecond * DateTime.Now.Millisecond;
            Temp.nType = (int)PipeMessage.__eMessageType.eSystem;
            Temp.nFrom = Process.GetCurrentProcess().Id;
            Temp.nTo = m_nServerID;
            Help.StrCopy(ref Temp.Message, "OK");
            Send(Temp);

            // мы получили запрос на чтото
            // например выслать наши настройки
            // пока не знаю как это будет выглядеть :)
            if (Message.nResponceID == 0)
            {

            }
        }

        //! Подключиться к pipe
        public bool Connect()
        {
            string sPipeName = "\\\\.\\pipe\\RECIEVE" + m_nServerID.ToString();

            SafeFileHandle hPipe = Win32.CreateFile(
                sPipeName,                  // pipe name 
                Win32.GENERIC_READ |    // read and write access 
                Win32.GENERIC_WRITE,
                0,                      // no sharing 
                IntPtr.Zero,                      // default security attributes
                Win32.OPEN_EXISTING,    // opens existing pipe 
                0,                      // default attributes 
                IntPtr.Zero);           // no template file 

            // стримы через которые идет чтение/запись
            try
            {
                m_Stream = new FileStream(hPipe, FileAccess.ReadWrite, m_nBufSize, false);
            }
            catch (Exception)
            {
                return false;
            }

            return !hPipe.IsInvalid;
        }

        //! Отправить команду
        public PipeMessage Command(string sCommand)
        {
            if (sCommand.Length == 0)
                return new PipeMessage();

            // Формируем команду
            PipeMessage Message = new PipeMessage();
            Message.nID = DateTime.Now.Millisecond * DateTime.Now.Millisecond;
            Message.nResponceID = 0;
            Message.nFrom = Process.GetCurrentProcess().Id;
            Message.nTo = m_nServerID;
            Message.nType = (int)PipeMessage.__eMessageType.eSystem;
            if ("Set" == sCommand)
            {
                Message.nType = (int)PipeMessage.__eMessageType.eSettings;
                Message.Settings = m_Settings.Get();
            }
            Help.StrCopy(ref Message.Message, sCommand);

            // Ждем пайп
            while (!IsServerCreated())
                Thread.Sleep(100);

            // Коннектимся к пайпу
            if (!Connect())
                return new PipeMessage();

            // Отсылаем
            Send(Message);

            byte[] TempBuffer = new byte[m_nBufSize];
            // Ждем ответ
            int nBytesRead = m_Stream.Read(TempBuffer, 0, m_nBufSize);
            // Принимаем
            if (nBytesRead > 0)
                // Конвертируем обратно в структуру
                Message = ByteArrayToMessage(TempBuffer);

            m_Stream.Close();
            m_Stream.Dispose();
            return Message;
        }

        //! Конвертируем месседж в массив байт
        static byte[] MessageToByteArray(PipeMessage Msg)
        {
            object obj = (object)Msg;
            int len = Marshal.SizeOf(obj);
            byte[] arr = new byte[len];
            IntPtr ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, len);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        //! Конвертируем массив байт в месседж
        static PipeMessage ByteArrayToMessage(byte[] bytearray)
        {
            PipeMessage Msg = new PipeMessage();
            object obj = (object)Msg;
            int len = Marshal.SizeOf(obj);
            IntPtr i = Marshal.AllocHGlobal(len);
            Marshal.Copy(bytearray, 0, i, len);
            obj = Marshal.PtrToStructure(i, obj.GetType());
            Marshal.FreeHGlobal(i);
            return (PipeMessage)obj;
        }
    }
}
