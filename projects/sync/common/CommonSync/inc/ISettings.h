#ifndef ISettings_h__
#define ISettings_h__

#include <string>

//! Settings interface
class ISettings
{
public:

	//! Get db path
	virtual const std::string&		DbPath() const = 0;

	//! Get local host guid 
	virtual const std::string&		LocalGuid() const = 0;

	//! Get ping interval
	virtual const std::size_t		PingInterval() const = 0;

	//! Get threads count
	virtual const std::size_t		ThreadsCount() const = 0;

	//! Get UDP port
	virtual const std::size_t		UDPPort() const = 0;

	//! Get Buffer size
	virtual const std::size_t		BufferSize() const = 0;

	//! Get default job timeout
	virtual const std::size_t		JobTimeout() const = 0;
};

#endif // ISettings_h__
