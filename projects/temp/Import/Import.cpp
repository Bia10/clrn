// Import.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>

#include <vector>
#include <string>
#include <memory>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/foreach.hpp>

#define for_each BOOST_FOREACH

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
		reinterpret_cast<LPSTR>(&lpMsgBuf),
		0,
		NULL 
	);

	const std::string sResult(reinterpret_cast<const char*> (lpMsgBuf));

	// Free the buffer.
	LocalFree(lpMsgBuf);
	return sResult;
}

#define WINAPICHECKTHROW(expr) if (!(expr)) { std::ostringstream oss; \
	oss << "Error: File: " << __FILE__ << " Line: " << __LINE__ << "\nExpression: " << #expr " failed\nDsc:" << GetLastErrorDesc(); \
	throw std::runtime_error(oss.str().c_str()); \
}

template<typename T>
class ScopedHandle
{
public:

	//! Pointer type
	typedef boost::shared_ptr< ScopedHandle<T> > Ptr;

	explicit ScopedHandle(const T& h)
		: m_Handle(h)
	{
		WINAPICHECKTHROW(m_Handle != INVALID_HANDLE_VALUE);
	}

	operator T () const 
	{ 
		return m_Handle; 
	}

	~ScopedHandle()
	{
		CloseHandle(m_Handle);
	}

private:

	ScopedHandle(const ScopedHandle&);
	ScopedHandle& operator = (const ScopedHandle);

	//! Handle 
	T m_Handle;
};


struct Data
{
	std::string string;
	std::vector<char> data;
};

boost::thread_group				pool;
std::size_t						jobCount = 0;
ScopedHandle<HANDLE>::Ptr		hIOCP;

void WorkLoop()
{
	DWORD nBytes = 0;
	unsigned long pData = 0;
	LPOVERLAPPED RecvOverlapped;

	static std::size_t thread = 0;
	static boost::mutex mxth;
	std::size_t current = 0;
	{
		boost::mutex::scoped_lock lock(mxth);
		current = ++thread;
	}
	try
	{
		while(true)
		{
			WINAPICHECKTHROW(GetQueuedCompletionStatus(*hIOCP, &nBytes, &pData, &RecvOverlapped, INFINITE));
			boost::this_thread::interruption_point();

			const boost::shared_ptr<Data> data(reinterpret_cast<Data*>(pData));

			static boost::mutex mx;
			boost::mutex::scoped_lock lock(mx);
			std::cout << current << " work is done!: " << jobCount++ << " " << data->string << std::endl;
		}
	}
	catch (const boost::thread_interrupted&)
	{

	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

class Object
{
public:

	Object()
		: m(0)
	{
		std::cout << "Object()" << std::endl;

	}

	~Object()
	{
		std::cout << "~Object()" << std::endl;
	}

	Object(const Object& obj)
		: m(obj.m)
	{
		//std::cout << "Object(const Object& obj)" << std::endl;
	}

	const Object& operator = (const Object& obj)
	{
		m = obj.m;
		std::cout << "const Object& operator = (const Object& obj)" << std::endl;
	}

	void Do()
	{
		++m;
		boost::this_thread::sleep(boost::posix_time::microseconds(1));
	}

private:

	int m;
};

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		hIOCP.reset(new ScopedHandle<HANDLE>(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL)));

		for (std::size_t i = 0; i < 1; ++i)
		{
			pool.create_thread(boost::bind(&WorkLoop));
		}

		for (std::size_t i = 0; i < 20; ++i)
		{
			OVERLAPPED RecvOverlapped;
			ZeroMemory(&RecvOverlapped, sizeof(RecvOverlapped));

			std::auto_ptr<Data> data(new Data);
			data->data.resize(10, i);
			data->string.resize(10, i + 50);

			WINAPICHECKTHROW(PostQueuedCompletionStatus(*hIOCP, 0, reinterpret_cast<ULONG_PTR>(data.get()), 0));
			data.release();
		}

		pool.join_all();

	}
	catch (const std::exception& e)
	{	
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{

	}

	return 0;
}

