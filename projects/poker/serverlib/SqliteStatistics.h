#ifndef SqliteStatistics_h__
#define SqliteStatistics_h__

#include "IStatistics.h"

namespace srv
{

//! Stats writer/reader
class SqliteStatistics : public IStatistics
{
public:

	SqliteStatistics(ILog& logger);
	~SqliteStatistics();

	virtual void Write(pcmn::TableContext::Data& data) override;
	virtual unsigned GetRanges(PlayerInfo::List& players) const override;
	virtual void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) const override;
	virtual unsigned GetEquities(PlayerInfo::List& players) const override;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv
#endif // SqliteStatistics_h__
