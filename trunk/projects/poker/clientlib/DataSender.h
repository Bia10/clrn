#ifndef DataSender_h__
#define DataSender_h__

#include "IDataSender.h"
#include "ILog.h"

#include <vector>

#include <boost/shared_ptr.hpp>

namespace clnt
{

class DataSender : public pcmn::IDataSender
{
	typedef std::vector<char> Buffer;
	typedef boost::shared_ptr<Buffer> BufferPtr;
public:
	DataSender(ILog& logger);
	~DataSender();
private:
	virtual void OnGameFinished(const net::Packet& packet) override;
private:
	class Impl;
	Impl* m_Impl;
};

} // namespace clnt


#endif // DataSender_h__
