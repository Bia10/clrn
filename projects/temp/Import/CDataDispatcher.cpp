#include "CDataDispatcher.h"

#include <Windows.h>

#include <map>
#include <deque>
#include <vector>

#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


struct CChannelPacketsFilter
{

};

class DataDispatcher::Impl
{
public:

	struct CallBackInfo
	{
		DispatchCallbackFn	callback;
		HANDLE				eventHandle;
		DWORD				channel;

	};

	//! Waiting callback queue
	typedef std::deque<DispatchCallbackFn>	CallbackQueue;

	//! Waiting queues
	typedef std::map<DWORD, CallbackQueue>	WaitingQueues;

	//! Channels map
	typedef std::map<unsigned char, DWORD>	ChannelsMap;

	//! Thread events
	typedef std::vector<HANDLE>				EventsPool;

	//! Data receive callbacks
	typedef std::pair<CChannelPacketsFilter, DataReceivedCallbackFn> DataCallback;

	//! Data callbacks list
	typedef std::list<DataCallback>			DataCallBackList;

	Impl(IKernel& kernel, unsigned char ownerPlugin, std::size_t threadsCount)
		: m_CurrentEvent(0)
		, m_ThreadsCount(threadsCount)
		, m_OwnerPlugin(ownerPlugin)
	{
		for (std::size_t i = 0 ; i < m_ThreadsCount; ++i)
		{
			const HANDLE event = CreateEvent(NULL, false, false, NULL);
			m_EventsPool.push_back(event);
			m_ThreadPool.create_thread(boost::bind(&Impl::WorkLoop, this, event));
		}
	}

	void WorkLoop(const HANDLE event)
	{
		for (;;)
		{
			WaitForSingleObject(event, INFINITE);

			// read data for this plugin m_OwnerPlugin
			std::vector<char> Packet;
			const DWORD channel = 0;

			DataPtr data(new BufferHolder(/* packet */));

			// getting callback for this channel
			const DispatchCallbackFn functor = GetNextCallBack(channel);

			if (functor)
				functor(data);

			// find appropriate callback for this channel info
			// m_DataCallbacks
			m_DataCallbacks.front().second(data, 0); // get plugin id from channel info
		}
	}
	
	DispatchCallbackFn GetNextCallBack(const DWORD channel)
	{
		boost::mutex::scoped_lock lock(m_QueueMutex);

		DispatchCallbackFn result;
		if (!m_WaitingCallbacks.count(channel))
			return result;

		if (m_WaitingCallbacks[channel].empty())
		{
			m_WaitingCallbacks.erase(channel);
			return result;
		}

		result = m_WaitingCallbacks[channel].front();
		m_WaitingCallbacks[channel].pop_front();
		return result;
	}

	DWORD GetChannel(unsigned char plugin)
	{
		return m_Channels[plugin];
	}

	void Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, const DispatchCallbackFn& callback)
	{
		boost::mutex::scoped_lock lock(m_CallbacksMutex);

		const DWORD channel = GetChannel(targetPlugin);
		m_WaitingCallbacks[channel].push_back(callback);

		// send data
	}

	void EventSetCallBack(const HANDLE event, DataPtr& result, const DataPtr& buffer)
	{
		SetEvent(event);
		result = buffer;
	}

	DataDispatcher::DataPtr Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, unsigned long timeout)
	{
		// send data
	
		HANDLE event = INVALID_HANDLE_VALUE;
		DataDispatcher::DataPtr result;

		{
			// registering in request queue
			boost::mutex::scoped_lock lock(m_CallbacksMutex);

			const DWORD channel = GetChannel(targetPlugin);

			// create sync handle
			event = CreateEvent(NULL, false, false, NULL);

			// queue fake callback
			m_WaitingCallbacks[channel].push_back(boost::bind(&Impl::EventSetCallBack, this, event, result, _1));		
		}

		// wait for event
		WaitForSingleObject(event, timeout);

		// return buffer containing received data
		return result;
	}

	void OnDataArrived()
	{
		const HANDLE event = GetNextEvent();
		SetEvent(event);
	}

	HANDLE GetNextEvent()
	{
		boost::mutex::scoped_lock lock(m_EventMutex);
		const HANDLE result = m_EventsPool[m_CurrentEvent];
		if (++m_CurrentEvent == m_ThreadsCount)
			m_CurrentEvent = 0;
		return result;
	}

private:

	//! Event index
	std::size_t			m_CurrentEvent;

	//! Threads count
	std::size_t			m_ThreadsCount;

	//! Plugin owner
	unsigned char		m_OwnerPlugin;

	//! Created channels
	ChannelsMap			m_Channels;

	//! Waiting callbacks
	WaitingQueues		m_WaitingCallbacks;

	//! Thread pool
	boost::thread_group	m_ThreadPool;

	//! Events pool
	EventsPool			m_EventsPool;

	//! Events mutex
	boost::mutex		m_EventMutex;

	//! Queue mutex
	boost::mutex		m_QueueMutex;

	//! Callbacks mutex
	boost::mutex		m_CallbacksMutex;

	//! Data callbacks
	DataCallBackList	m_DataCallbacks;
};


DataDispatcher::DataDispatcher(IKernel& kernel, unsigned char ownerPlugin, std::size_t threadsCount /*= 1*/)
	: m_pImpl(new Impl(kernel, ownerPlugin, threadsCount))
{
}

DataDispatcher::~DataDispatcher(void)
{
}

void DataDispatcher::Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, const DispatchCallbackFn& callback)
{
	m_pImpl->Dispatch(targetPlugin, data, size, callback);
}

DataDispatcher::DataPtr DataDispatcher::Dispatch(unsigned char targetPlugin, const void* data, std::size_t size, unsigned long timeout)
{
	return m_pImpl->Dispatch(targetPlugin, data, size, timeout);
}

void DataDispatcher::OnDataArrived()
{
	m_pImpl->OnDataArrived();
}