#include "ActionsDetector.h"

namespace srv
{

ActionsDetector::ActionsDetector(ILog& logger, const IStatistics& stats) : m_Log(logger), m_Stats(stats)
{

}


void ActionsDetector::DetectAtions(const std::string& name, pcmn::Player::Position::Value pos, const InputHands& hands, unsigned street, unsigned count, pcmn::Board::Value board, OutputActions& result)
{
    IStatistics::PlayerInfo player;
    player.m_Name = name;
    player.m_Position = pos;

    m_Stats.GetActions(player, board, street, count);
}

} // namespace srv