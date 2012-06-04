#pragma once

#include <memory>

#include <boost/function/function_fwd.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// Forward declarations
class IKernel;
struct CChannelPacketsFilter;

//! Class for data synchronous/asynchronous transfers
//!
//! \class DataDispatcher
//!
class DataDispatcher 
	: private boost::noncopyable
{
public:

	//! Data descriptor
	struct BufferHolder
	{
		const void* data;
		std::size_t size;
	};

	//! Data holder pointer
	typedef boost::shared_ptr<BufferHolder> DataPtr;

	//! Data dispatch callback type
	typedef boost::function<void (const DataPtr&)>					DispatchCallbackFn;

	//! Data receive callback type
	typedef boost::function<void (const DataPtr&, unsigned char)>	DataReceivedCallbackFn;

	DataDispatcher(IKernel& kernel, unsigned char ownerPlugin, std::size_t threadsCount = 1);
	~DataDispatcher(void);

	//! On data handle(this method must be called in IPlugin::handle())
	void		OnDataArrived();

	//! Async dispatch
	void		Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, const DispatchCallbackFn& callback);
	void		Dispatch(unsigned char targetPlugin, const DataPtr& buffer, const DispatchCallbackFn& callback);

	//! Sync dispatch
	DataPtr		Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, unsigned long timeout = 0xFFFFFFFF);
	DataPtr		Dispatch(unsigned char targetPlugin, const DataPtr& buffer, unsigned long timeout = 0xFFFFFFFF);

	//! Set callback with specified filter
	void		SetCallback(const CChannelPacketsFilter& filter, const DataReceivedCallbackFn& callback);

	//! Resize thread pool
	void		SetThreadsCount(std::size_t count);

private:

	//! Implementation
	class Impl;
	std::auto_ptr<Impl> m_pImpl;

};
