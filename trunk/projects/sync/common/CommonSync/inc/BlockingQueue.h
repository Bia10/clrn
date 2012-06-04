#ifndef BlockingQueue_h__
#define BlockingQueue_h__

#include <deque>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

//! Custom blocking queue implementation
//! 
//! \class JobQueue
//!
template<typename T>
class CBlockingQueue
{
	//! Internal queue type
	typedef std::deque<T> QueueType;

public:
	CBlockingQueue(void)
	{

	}

	~CBlockingQueue(void)
	{

	}

	//! Queue data
	void            PushFront(const T& data)
	{
		boost::lock_guard<boost::mutex> lock(m_QueueMutex);
		m_Queue.push_front(data);
		m_NewItem.notify_one();
	}

	//! Queue data
	void            PushBack(const T& data)
	{
		boost::lock_guard<boost::mutex> lock(m_QueueMutex);
		m_Queue.push_back(data);
		m_NewItem.notify_one();
	}

	//! Get next data
	T   PopFront()
	{
		boost::unique_lock<boost::mutex> lock(m_QueueMutex);
		while (m_Queue.empty())
		{
			m_NewItem.wait(lock);
			boost::this_thread::interruption_point();
		}

		const T result = m_Queue.front();
		m_Queue.pop_front();
		return result;
	}

	//! Get next data
	T   PopBack()
	{
		boost::unique_lock<boost::mutex> lock(m_QueueMutex);
		while (m_Queue.empty())
		{
			m_NewItem.wait(lock);
			boost::this_thread::interruption_point();
		}

		const T result = m_Queue.back();
		m_Queue.pop_back();
		return result;
	}

	//! Clear all data
	void   Clear()
	{
		boost::unique_lock<boost::mutex> lock(m_QueueMutex);
		m_Queue.clear();
		m_NewItem.notify_all();
	}

	//! Queue size
	std::size_t  Size() const
	{
		return m_Queue.size();
	}

private:

	//! Queue
	QueueType     m_Queue;

	//! Queue mutex
	boost::mutex                m_QueueMutex;

	//! New item condition
	boost::condition_variable m_NewItem;
};
#endif // BlockingQueue_h__