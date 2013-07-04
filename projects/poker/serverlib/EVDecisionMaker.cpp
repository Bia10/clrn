#include "EVDecisionMaker.h"

namespace srv
{

EVDecisionMaker::EVDecisionMaker(ILog& logger, const pcmn::Evaluator& evaluator, IStatistics& stats, net::IConnection& connection)
    : m_Log(logger)
    , m_Evaluator(evaluator)
    , m_Stat(stats)
    , m_Connection(connection)
{

}

void EVDecisionMaker::MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position)
{
    // 1. сделать расчет EV турнира, принять константными значения за первые 3 места
    // 2. для каждого возможного действия  и каждой возможной руки каждого соперника считаем EV
}

void EVDecisionMaker::SendRequest(const net::Packet& /*packet*/, bool /*statistics*/)
{
    throw std::exception("The method or operation is not implemented.");
}

void EVDecisionMaker::WriteStatistics(pcmn::TableContext::Data& data)
{
    m_Stat.Write(data);
}

} // namespace srv