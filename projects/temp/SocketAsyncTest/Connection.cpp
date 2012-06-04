#include "Connection.h"

#include <stdexcept>
#include <sstream>
#include <memory>

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

std::string GetLastErrorDesc() 
{
	LPVOID lpMsgBuf;
	DWORD l_err = GetLastError();
	FormatMessageA
	( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		l_err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR) &lpMsgBuf,
		0,
		NULL 
	);

	const std::string sResult(reinterpret_cast< const char * > (lpMsgBuf));

	// Free the buffer.
	LocalFree(lpMsgBuf);
	return sResult;
}

#define WINAPICHECKTHROW(expr) if (expr) { std::ostringstream oss; \
	oss << "Error: File: " << __FILE__ << " Line: " << __LINE__ << "\nExpression: " << #expr " failed\nDsc:" << GetLastErrorDesc(); \
	throw std::runtime_error(oss.str().c_str()); \
}

//! Implementation of the CConnection class
class CConnection::CConnectionImpl
{
private:

	//! Registered callbacks type
	typedef std::map< CEvent::Enum_t, TCallback > TCallBacks;

public:
	CConnectionImpl()
		: m_nNextIOCP(0)
		, m_pfnAcceptEx(0)
	{
		boost::mutex::scoped_lock Lock(m_mxInstanceCount);
		if (0 == ms_nInstanceCount)
			Init();

		++ms_nInstanceCount;
	}

	~CConnectionImpl()
	{
		boost::mutex::scoped_lock Lock(m_mxInstanceCount);
		--ms_nInstanceCount;

		if (0 == ms_nInstanceCount)
			Uninit();
	}

	void							RegisterCallBack(const CEvent::Enum_t eEvent, const TCallback& fCallback)
	{
		if (m_mapCallBacks.count(eEvent))
			throw std::runtime_error("Event already exists.");

		m_mapCallBacks.insert(std::make_pair(eEvent, fCallback));
	}


	//! Initialization
	void							Init()
	{
		// Initialize Winsock
		WSADATA wsaData;
		WINAPICHECKTHROW(WSAStartup(MAKEWORD(2,2), &wsaData) != 0);
	}

	//! Deinitialization
	void							Uninit()
	{
		WSACleanup();
	}

	SOCKET							CreateListeningSocket(const std::size_t nPort)
	{
		SOCKET ResultSocket = INVALID_SOCKET;
		struct addrinfo *result = NULL;  // connection info
		addrinfo hints;

		try
		{
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family		= AF_INET;
			hints.ai_socktype	= SOCK_STREAM;
			hints.ai_protocol	= IPPROTO_TCP;
			hints.ai_flags		= AI_PASSIVE;

			// Resolve the server address and port
			const std::string sPort = boost::lexical_cast< std::string > (nPort);
			WINAPICHECKTHROW(getaddrinfo(NULL, sPort.c_str(), &hints, &result) != 0);

			// Create a SOCKET for connecting to server
			ResultSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			WINAPICHECKTHROW(ResultSocket == INVALID_SOCKET);

			// Setup the TCP listening socket
			WINAPICHECKTHROW(bind(ResultSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR);

			WINAPICHECKTHROW(listen(ResultSocket, SOMAXCONN) == SOCKET_ERROR);

			// Getting AcceptEx pointer
			GUID GuidAcceptEx = WSAID_ACCEPTEX;
			DWORD dwBytes = 0;
			WINAPICHECKTHROW
			(
				WSAIoctl
				(
					ResultSocket, 
					SIO_GET_EXTENSION_FUNCTION_POINTER,
					&GuidAcceptEx, 
					sizeof (GuidAcceptEx), 
					&m_pfnAcceptEx, 
					sizeof (m_pfnAcceptEx), 
					&dwBytes, 
					NULL, 
					NULL
				) == SOCKET_ERROR
			);

		}
		catch (std::exception&)
		{
			freeaddrinfo(result);
			throw;
		}
		return ResultSocket;
	}

	void							StartAccept(const SOCKET ListenSocket)
	{
		// Creating socket for client
		SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		WINAPICHECKTHROW(ClientSocket == INVALID_SOCKET);

		// Getting next IOCP handle
		const HANDLE hIOCP = GetNextIOCP();

		// Associating IOCP handle with client handle
		std::auto_ptr< CIOInfo > pInfo(new CIOInfo());
		pInfo->eEvent = CEvent::eConnect;
		pInfo->Socket = ClientSocket;
		HANDLE hPort = CreateIoCompletionPort
		(
			reinterpret_cast< HANDLE > (ListenSocket), 
			hIOCP, 
			reinterpret_cast< ULONG_PTR > (pInfo.get()),
			m_ThreadPool.size()
		);

		WINAPICHECKTHROW(hPort == NULL);

		WSAOVERLAPPED olOverlap;
		memset(&olOverlap, 0, sizeof (olOverlap));

		char Dummy;
		DWORD dwBytes = 0;

		// Starting async accept
		BOOL bRetVal = m_pfnAcceptEx
		(
			ListenSocket, 
			ClientSocket, 
			&Dummy,
			0,
			sizeof (sockaddr_in) + 16, 
			sizeof (sockaddr_in) + 16, 
			&dwBytes, 
			&olOverlap
		);
		
		WINAPICHECKTHROW(bRetVal == FALSE && WSA_IO_PENDING != WSAGetLastError());
		pInfo.release();
	}

	void							WorkLoop(const HANDLE hIOCP)
	{
		DWORD nBytes = 0;
		unsigned long pData = 0;
		LPOVERLAPPED RecvOverlapped;

		// Getting callbacks
		const TCallBacks::const_iterator itConnect = m_mapCallBacks.find(CEvent::eConnect);
		const TCallBacks::const_iterator itRead = m_mapCallBacks.find(CEvent::eRead);
		const TCallBacks::const_iterator itWrite = m_mapCallBacks.find(CEvent::eWrite);
		const TCallBacks::const_iterator itEnd = m_mapCallBacks.end();

		try
		{
			while(true)
			{
				boost::this_thread::interruption_point();

				if(!GetQueuedCompletionStatus(hIOCP, &nBytes, &pData, &RecvOverlapped, INFINITE))
					break;

				const std::auto_ptr< CIOInfo > pInfo(reinterpret_cast< CIOInfo* >(pData));
				switch (pInfo->eEvent)
				{
				case CEvent::eConnect:
					if (itConnect != itEnd)
						itConnect->second(*pInfo);
					break;
				case CEvent::eRead:
					if (itRead != itEnd)
						itRead->second(*pInfo);
					break;
				case CEvent::eWrite:
					if (itWrite != itEnd)
						itWrite->second(*pInfo);
					break;
				}
			}
		}
		catch (boost::thread_interrupted&)
		{
			
		}
	}

	void							CreateThreadPool(const std::size_t nSize)
	{
		for (std::size_t i = 0; i < nSize; ++i)
		{
			// Create new IO completion port
			HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
			WINAPICHECKTHROW(hIOCP == NULL);
			m_vecIOCP.push_back(hIOCP);

			// Create new thread associated with IOCP handle
			m_ThreadPool.create_thread(boost::bind(&CConnectionImpl::WorkLoop, this, hIOCP));
		}
	}

	HANDLE							GetNextIOCP()
	{
		static boost::mutex s_mxIOCP;

		boost::mutex::scoped_lock Lock(s_mxIOCP);
		HANDLE hIOCP = m_vecIOCP[m_nNextIOCP++];

		if (m_nNextIOCP == m_ThreadPool.size())
			m_nNextIOCP = 0;

		return hIOCP;
	}

private:

	//! Callbacks
	TCallBacks						m_mapCallBacks;

	//! Connection instance count
	static volatile std::size_t		ms_nInstanceCount;

	//! Instance count mutex
	boost::mutex					m_mxInstanceCount;

	//! Thread pool
	boost::thread_group				m_ThreadPool;

	//! IOCP pool
	std::vector< HANDLE >			m_vecIOCP;

	//! Next IOCP index
	std::size_t						m_nNextIOCP;

	//! AcceptEx function pointer
	LPFN_ACCEPTEX					m_pfnAcceptEx;
};


volatile std::size_t CConnection::CConnectionImpl::ms_nInstanceCount = 0;

CConnection::CConnection() 
	: m_pImpl(new CConnectionImpl())
{
	
}

CConnection::~CConnection(void)
{
	delete m_pImpl;
	m_pImpl = 0;
	closesocket(m_nSocket);
}

void CConnection::RegisterCallback(const CEvent::Enum_t eEvent, const TCallback& fCallback)
{
	m_pImpl->RegisterCallBack(eEvent, fCallback);
}

void CConnection::Create(const std::size_t nPort)
{
	m_nSocket = m_pImpl->CreateListeningSocket(nPort);
	m_pImpl->StartAccept(m_nSocket);
}

void CConnection::Connect(const std::string& sHost, const std::size_t nPort)
{

}

void CConnection::Close()
{

}

void CConnection::Run( const std::size_t nThreadsPoolSize )
{
	m_pImpl->CreateThreadPool(nThreadsPoolSize);
}

void CConnection::Stop()
{
	
}