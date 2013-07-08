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
    // полное мат. ожидание - сумма мат ожиданий по каждому игроку
    // мат ожидание по игроку - сумма всех исходов 
    // для каждой возможной ставки считаем подное мат. ожидание, берем максимальное значение

    // мат. ожидание для bet:
    // для каждого игрока мат. ожидание это сумма мат. ожиданий для всех его предполагаемых рук умноженное на вероятность руки:
    // M[player] = ∑ P(hand) * M[hand]

    // мат. ожидание руки считаетя как:
    // M[hand] = M[call] + M[fold] + M[raise]
    // M[call] = ((банк + ставка бота + колл оппонента) * вероятность выигрыша бота + (- ставка бота) * вероятность проигрыша бота) * вероятность колла
    // M[fold] = (банк / кол-во оппонентов) * вероятность фолда
    // M[raise] = (расчитывается мат. ожидание  бота для рейза High) * вероятность рейза

    // мат. ожидание для call
    // для каждой предполагаемой руки оппонента:
    // M[player] = ∑ P(hand) * M[hand]
    // M[hand] = (размер банка + размер ставки оппонента) * вероятность выигрыша бота + (- размер ставки оппонента) * вероятность проигрыша бота
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