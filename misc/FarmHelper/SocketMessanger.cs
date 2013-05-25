using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace FarmHelper
{
    class CSocketMessanger
    {
        public CSocketMessanger(string sCompName, int nPort, int nServerID)
        {
            m_SockType = SocketType.Stream;
            m_SockProtocol = ProtocolType.Tcp;
            m_sRemoteName = sCompName;
            m_nBufferSize = 4096;
            m_nRemotePort = nPort;
            m_bUdpConnect = false;
            m_ClientSocket = null;
            m_ResolvedHost = null;
            m_Destination = null;
            m_Buffer = new byte[m_nBufferSize];
            m_nServerID = nServerID;
            m_Data = new CData();
            m_Settings = new CSettings();
            CSettings.SettingsStruct ss = new CSettings.SettingsStruct();
            ss.m_nLogLevel = 1;
            m_Settings.Set(ss);
            
        }

        //! Данные из сервера
        CData m_Data;

        //! Настройки
        CSettings m_Settings;

        SocketType m_SockType;
        ProtocolType m_SockProtocol;
        string m_sRemoteName;
        int m_nRemotePort;
        int m_nBufferSize;
        bool m_bUdpConnect;
        Socket m_ClientSocket;
        IPHostEntry m_ResolvedHost;
        IPEndPoint m_Destination;
        byte[] m_Buffer;
        PipeMessage Message;
        int m_nServerID;

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

        public PipeMessage Send(string sCommand)
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

            m_Buffer = MessageToByteArray(Message);
            //FormatBuffer(m_Buffer, sCommand);
            int rc = 0;
            try
            {
                // Try to resolve the remote host name or address
                m_ResolvedHost = Dns.GetHostEntry(m_sRemoteName);
                Console.WriteLine("Client: GetHostEntry() is OK...");

                // Try each address returned
                foreach (IPAddress addr in m_ResolvedHost.AddressList)
                {
                    // Create a socket corresponding to the address family of the resolved address
                    m_ClientSocket = new Socket(
                        addr.AddressFamily,
                        m_SockType,
                        m_SockProtocol
                        );
                    Console.WriteLine("Client: Socket() is OK...");
                    try
                    {
                        // Create the endpoint that describes the destination
                        m_Destination = new IPEndPoint(addr, m_nRemotePort);
                        Console.WriteLine("Client: IPEndPoint() is OK. IP Address: {0}, server port: {1}", addr, m_nRemotePort);

                        if ((m_SockProtocol == ProtocolType.Udp) && (m_bUdpConnect == false))
                        {
                            Console.WriteLine("Client: Destination address is: {0}", m_Destination.ToString());
                            break;
                        }
                        else
                        {
                            Console.WriteLine("Client: Attempting connection to: {0}", m_Destination.ToString());
                        }
                        m_ClientSocket.Connect(m_Destination);
                        Console.WriteLine("Client: Connect() is OK...");
                        break;
                    }
                    catch (SocketException)
                    {
                        // Connect failed, so close the socket and try the next address
                        m_ClientSocket.Close();
                        Console.WriteLine("Client: Close() is OK...");
                        m_ClientSocket = null;
                        continue;
                    }
                }

                // Make sure we have a valid socket before trying to use it
                if ((m_ClientSocket != null) && (m_Destination != null))
                {
                    try
                    {
                        // Send the request to the server
                        if ((m_SockProtocol == ProtocolType.Udp) && (m_bUdpConnect == false))
                        {
                            m_ClientSocket.SendTo(m_Buffer, m_Destination);
                            Console.WriteLine("Client: SendTo() is OK...UDP...");
                        }
                        else
                        {
                            rc = m_ClientSocket.Send(m_Buffer);
                            Console.WriteLine("Client: send() is OK...TCP...");
                            Console.WriteLine("Client: Sent request of {0} bytes", rc);

                            // For TCP, shutdown sending on our side since the client won't send any more data
                            if (m_SockProtocol == ProtocolType.Tcp)
                            {
                                m_ClientSocket.Shutdown(SocketShutdown.Send);
                                Console.WriteLine("Client: Shutdown() is OK...");
                            }
                        }

                        // Receive data in a loop until the server closes the connection. For
                        //    TCP this occurs when the server performs a shutdown or closes
                        //    the socket. For UDP, we'll know to exit when the remote host
                        //    sends a zero byte datagram.
                        while (true)
                        {
                            if ((m_SockProtocol == ProtocolType.Tcp) || (m_bUdpConnect == true))
                            {
                                m_ClientSocket.ReceiveTimeout = 1000;
                                rc = m_ClientSocket.Receive(m_Buffer);
                                Console.WriteLine("Client: Receive() is OK...");
                                Console.WriteLine("Client: Read {0} bytes", rc);
                            }
                            else
                            {
                                IPEndPoint fromEndPoint = new IPEndPoint(m_Destination.Address, 0);
                                Console.WriteLine("Client: IPEndPoint() is OK...");
                                EndPoint castFromEndPoint = (EndPoint)fromEndPoint;
                                rc = m_ClientSocket.ReceiveFrom(m_Buffer, ref castFromEndPoint);
                                Console.WriteLine("Client: ReceiveFrom() is OK...");
                                fromEndPoint = (IPEndPoint)castFromEndPoint;
                                Console.WriteLine("Client: Read {0} bytes from {1}", rc, fromEndPoint.ToString());
                            }

                            // Exit loop if server indicates shutdown
                            if (rc == 0)
                            {
                                m_ClientSocket.Close();
                                Console.WriteLine("Client: Close() is OK...");
                                break;
                            }
                            else
                            {
                                Message = ByteArrayToMessage(m_Buffer);
                                return Message;
                            }
                        }
                    }
                    catch (SocketException err)
                    {
                        Console.WriteLine("Client: Error occurred while sending or receiving data.");
                        Console.WriteLine("   Error: {0}", err.Message);
                    }
                }
                else
                {
                    Console.WriteLine("Client: Unable to establish connection to server!");
                }
            }
            catch (SocketException err)
            {
                Console.WriteLine("Client: Socket error occurred: {0}", err.Message);
            }
            return new PipeMessage();
        }
    }
}
