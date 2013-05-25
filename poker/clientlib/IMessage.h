#ifndef IMessage_h__
#define IMessage_h__

#include <cstddef>

#include <boost/noncopyable.hpp>

namespace dasm
{
	struct WindowMessage;
}

namespace clnt
{

class ITable;

//! Poker message interface
class IMessage : boost::noncopyable
{
public:

	virtual ~IMessage() {}
	virtual std::size_t GetId() const = 0;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const = 0;
};

}


#endif // IMessage_h__
