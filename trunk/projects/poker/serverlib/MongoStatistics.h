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
	virtual pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent) const override;
	virtual void GetEquities(PlayerInfo::List& players, unsigned street) const override;
    virtual void GetHands(PlayerInfo& player, unsigned street, unsigned count) const override;
    virtual void GetActions(PlayerInfo& player, pcmn::Board::Value board, unsigned street, unsigned count) const override;
private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // MongoStatistics_h__