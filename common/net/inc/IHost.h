#ifndef IHost_h__
#define IHost_h__

#include "IConnection.h"

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace boost
{
    namespace asio
    {
        class io_service;
    }
}

namespace net
{

//! Remote host abstraction
class IHost
{
public:

    //! Service type
    typedef boost::asio::io_service Service;

	virtual ~IHost(){}

	//! Pointer type
	typedef boost::shared_ptr<IHost> Ptr;

	//! Callback function type
	typedef boost::function<void (const google::protobuf::Message& message, const IConnection::Ptr& connection)> Callback;

	//! Connect to remote host
	virtual IConnection::Ptr	Connect(const std::string& host, const short port) = 0;

	//! Receive callback
	virtual void				Receive(const Callback& callback, const google::protobuf::Message& message, const short port) = 0;

	//! Set buffer size
	virtual void				SetBufferSize(const int size) = 0;

	//! Run server until stopped
	virtual void				Run() = 0;

	//! Stop server
	virtual void				Stop() = 0;

	//! Wait for server stop
	virtual void				Wait() = 0;

    //! Get host service
    virtual Service&            GetService() = 0;
};


} // namespace net
#endif // IHost_h__
