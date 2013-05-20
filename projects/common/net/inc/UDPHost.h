#ifndef UDPHost_h__
#define UDPHost_h__

#include "IHost.h"
#include "ILog.h"

namespace net
{
class UDPHost : public IHost
{
public:
	UDPHost(ILog& logger, std::size_t threads);
	~UDPHost();
	virtual IConnection::Ptr	Connect(const std::string& host, const short port) override;
	virtual void				Receive(const Callback& callback, const google::protobuf::Message& message, const short port) override;
	virtual void				SetBufferSize(const int size) override;
	virtual void				Run() override;
	virtual void				Stop() override;
	virtual void				Wait() override;
    virtual Service&            GetService() override;
private:

	class Impl;
	Impl* m_Impl;
};
}

#endif // UDPHost_h__
