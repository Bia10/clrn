#ifndef IConnection_h__
#define IConnection_h__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace google 
{
namespace protobuf 
{
	class Message;
}
}

namespace net
{

//! Remote host connection abstraction
class IConnection : boost::noncopyable
{
public:
	virtual ~IConnection(){}

	//! Pointer type
	typedef boost::shared_ptr<IConnection> Ptr;

	//! Callback function type
	typedef boost::function<void (const google::protobuf::Message& packet)> Callback;

	//! Send to remote host
	virtual void Send(const google::protobuf::Message& message) = 0;

	//! Receive callback
	virtual void Receive(const Callback& callback, const google::protobuf::Message* message = 0) = 0;
};


} // namespace net


#endif // IConnection_h__
