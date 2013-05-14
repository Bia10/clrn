#ifndef MongoStatistics_h__
#define MongoStatistics_h__

#include "IStatistics.h"

namespace srv
{

//! Stats writer/reader
class MongoStatistics : public IStatistics
{
public:

	MongoStatistics(ILog& logger);
	~MongoStatistics();

	virtual void Write(pcmn::TableContext::Data& data) override;
	virtual unsigned GetRanges(PlayerInfo::List& players) const override;
	virtual void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) const override;
	virtual unsigned GetEquities(PlayerInfo::List& players, unsigned street) const override;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // MongoStatistics_h__
