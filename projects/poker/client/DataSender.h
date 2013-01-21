#ifndef DataSender_h__
#define DataSender_h__

#include "IDataSender.h"
#include "ILog.h"

namespace clnt
{

class DataSender : public pcmn::IDataSender
{
public:
	DataSender(ILog& logger);
private:
	virtual void OnGameFinished(const Statistic& stats) override;
};

} // namespace clnt


#endif // DataSender_h__
