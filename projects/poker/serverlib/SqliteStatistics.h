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
    virtual pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent) const override;
	virtual void GetEquities(PlayerInfo::List& players, unsigned) const override;
    virtual void GetHands(PlayerInfo& player, unsigned street, unsigned count) const override;

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv
#endif // SqliteStatistics_h__
