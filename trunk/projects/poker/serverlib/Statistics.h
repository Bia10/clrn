#ifndef Statistics_h__
#define Statistics_h__

#include "IStatistics.h"

namespace srv
{

//! Stats writer/reader
class Statistics : public IStatistics
{
public:

	Statistics(ILog& logger);
	~Statistics();

	virtual void Write(pcmn::TableContext::Data& data) override;
	virtual void GetRanges(PlayerRanges& players) override;
	virtual void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) override;
	virtual void GetEquities(PlayerEquities::List& players) override;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // Statistics_h__
