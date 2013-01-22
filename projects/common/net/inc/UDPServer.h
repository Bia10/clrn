#ifndef UDPServer_h__
#define UDPServer_h__

#include "ILog.h"
#include "google/protobuf/message.h"

#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function/function_fwd.hpp>

//! Forward declarations
namespace boost
{
	namespace asio
	{
		class io_service;
	}
}

namespace net
{

//! UDP server interface
//! 
//! \class CUDPServer
//!
class UDPServer : private boost::noncopyable
{
public:

	//! Reply interface
	class IClient
	{
	public:
		virtual ~IClient() {}
		virtual void SendReply(const google::protobuf::Message& message) const = 0;
	};

	//! Buffer type
	typedef std::vector<char> Buffer;

	//! Buffer pointer type
	typedef boost::shared_ptr<Buffer> BufferPtr;

	//! Reply callback type
	typedef boost::function<void (const BufferPtr& buffer, const IClient& client)> Callback;

	UDPServer(ILog& logger, const short port, const std::size_t threads, const Callback& callback);
	~UDPServer();

	//! Set buffer size
	void		SetBufferSize(const int size);

	//! Run server until stopped
	void		Run();

	//! Stop server
	void		Stop();

private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl> m_pImpl;
};

} // namespace net
#endif // UDPServer_h__
