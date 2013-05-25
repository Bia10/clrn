#pragma once

#include <string>
#include <map>

#include <boost/function/function_fwd.hpp>

//! Client connection
class CConnection
{
public:

	//! Events
	struct CEvent
	{
		enum Enum_t
		{
			eConnect,
			eRead,
			eWrite
		};
	};
	
	//! Buffer type
	struct CBuffer
	{
		void *		pData;		//!< Data pointer
		std::size_t nLength;	//!< Data size
	};

	//! IO operation info
	struct CIOInfo
	{
		std::size_t		Socket;
		CEvent::Enum_t	eEvent;
		CBuffer*		pBuffer;
		std::size_t		nOperationSize;
	};

	//! Callback function type
	typedef boost::function< void (CIOInfo&) > TCallback;

	CConnection();
	~CConnection(void);

	//! Start threads
	void							Run(const std::size_t nThreadsPoolSize);

	//! Stop threads	
	void							Stop();

	//! Creates listening socket on specified port and starts listening
	void							Create(const std::size_t nPort);

	//! Connects to specified server
	void							Connect(const std::string& sHost, const std::size_t nPort);

	//! Close
	void							Close();

	//! Register callback
	void							RegisterCallback(const CEvent::Enum_t eEvent, const TCallback& fCallback);


private:

	//! Connection implementation forward declaration
	class CConnectionImpl;

	//! Connection implementation
	CConnectionImpl*				m_pImpl;

	//! Socket
	unsigned int					m_nSocket;
};
