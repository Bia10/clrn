#ifndef IDataSender_h__
#define IDataSender_h__

#include <string>

namespace clnt
{

class IDataSender
{
public:
	struct Statistic
	{

	};

	virtual ~IDataSender() {}
	virtual void OnGameFinished(const Statistic& stats) = 0;
};
}

#endif // IDataSender_h__
