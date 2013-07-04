#include "ActionsDetector.h"

namespace srv
{

ActionsDetector::ActionsDetector(ILog& logger, const IStatistics& stats) : m_Log(logger), m_Stats(stats)
{

}


void ActionsDetector::GetFoldChance(const std::string& name, pcmn::Player::Position::Value pos, const InputHands& hands, unsigned street, unsigned count, pcmn::Board::Value board, Chances& result)
{
    IStatistics::PlayerInfo player;
    player.m_Name = name;
    player.m_Position = pos;

    m_Stats.GetActions(player, board, street, count);

    for (const auto& pair : player.m_Bets)
    {
        const auto& map = pair.second;
        const auto it = map.find(pcmn::Action::Fold);
        result.insert(std::make_pair(pair.first, it == map.end() ? 0.0f : it->second));
    }
}

} // namespace srv