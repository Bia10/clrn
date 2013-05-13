#include "DecisionMaker.h"
#include "Evaluator.h"
#include "TableContext.h"
#include "Config.h"
#include "BetSize.h"
#include "StackSize.h"
#include "../neuro/Params.h"
#include "packet.pb.h"
#include "Modules.h"
#include "Exception.h"

#include <cassert>
#include <sstream>

namespace srv
{

const int CURRENT_MODULE_ID = Modules::Server;

DecisionMaker::DecisionMaker
(
	ILog& logger, 
	const pcmn::Evaluator& evaluator, 
	srv::IStatistics& stats, 
	neuro::INetwork<float>& net,
	net::IConnection& connection
) 
	: m_Log(logger)
	, m_Evaluator(evaluator)
	, m_Stat(stats)
	, m_Net(net)
	, m_Connection(connection)
{
	 SCOPED_LOG(m_Log);
}

void DecisionMaker::SendRequest(const net::Packet& /*packet*/, bool /*statistics*/)
{
    throw std::exception("The method or operation is not implemented.");
}

void DecisionMaker::WriteStatistics(pcmn::TableContext::Data& data)
{
    m_Stat.Write(data);
}

void DecisionMaker::MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		assert(context.m_Pot);
		assert(context.m_BigBlind);
	
		std::vector<float> in;
		neuro::Params params;
	
		// win rate
		const float winRate = GetPlayerWinRate(player, context, activePlayers);
		params.m_WinRate = pcmn::WinRate::FromValue(winRate * 100);
		in.push_back(winRate);
	
		// position 
		params.m_Position = position;
		in.push_back(static_cast<float>(position) / pcmn::Player::Position::Max);
	
		// bet size
		params.m_BetSize = pcmn::BetSize::FromDecision(player.Bet(), context.m_MaxBet, context.m_Pot, player.Stack(), context.m_BigBlind);
		in.push_back(static_cast<float>(params.m_BetSize) / pcmn::BetSize::Max);
		
		// active players
		params.m_ActivePlayers = pcmn::Player::Count::FromValue(activePlayers.size() - 1);
		in.push_back(static_cast<float>(params.m_ActivePlayers) / pcmn::Player::Count::Max);
	
		// danger
		params.m_Danger = GetDanger(player, activePlayers, winRate * 100);
		in.push_back(static_cast<float>(params.m_Danger) / pcmn::Danger::Max);
	
		// bot average style
		params.m_BotAverageStyle = GetBotAverageStyle(player, activePlayers);
		in.push_back(static_cast<float>(params.m_BotAverageStyle) / pcmn::Player::Style::Max);
	
		// bot play style
		params.m_BotStyle = GetBotStyle(player);
		in.push_back(static_cast<float>(params.m_BotStyle) / pcmn::Player::Style::Max);
	
		// bot stack size
		params.m_BotStackSize = pcmn::StackSize::FromValue(player.Stack(), context.m_BigBlind, GetMaxStack(activePlayers));
		in.push_back(static_cast<float>(params.m_BotStackSize) / pcmn::StackSize::Max);

		std::vector<float> out;
		m_Net.Process(in, out);
	
		net::Reply reply;
	
		if (out[0] > out[1] && out[0] > out[2])
		{
			reply.set_amount(0);
			if (player.Bet() == context.m_MaxBet)
				reply.set_action(pcmn::Action::Check);
			else
				reply.set_action(pcmn::Action::Fold);
		}
		else
		if (out[1] > out[0] && out[1] > out[2])
		{
			if (context.m_MaxBet)
			{
				if (player.Stack() < context.m_BigBlind * 3)
				{
					reply.set_action(pcmn::Action::Raise);
					reply.set_amount(player.Stack());
				}
				else
				{
					reply.set_action(pcmn::Action::Call);
					reply.set_amount(context.m_MaxBet - player.Bet());
				}
			}
			else
			{
				reply.set_action(pcmn::Action::Check);
				reply.set_amount(0);
			}
		}
		else
		if (out[2] > out[0] && out[2] > out[1])
		{
			reply.set_action(pcmn::Action::Raise);

            unsigned amount = 0;
            if (!context.m_MaxBet)
            {
                amount = context.m_BigBlind * 5;
                if (amount < context.m_Pot) 
                    amount = context.m_Pot - context.m_Pot / 10;
            }
            else
            {
                 amount = context.m_MaxBet * 3;
            }

            amount = (amount / 10) * 10;

			if (amount > player.Stack() / 2)
				amount = player.Stack();

			reply.set_amount(amount);
		}

        if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
        {
            std::ostringstream oss;
            oss << "Cards: ";
            for (const pcmn::Card& card : player.Cards())
                oss << "('" << pcmn::Card::ToString(card.m_Value) << "' of '" << pcmn::Suit::ToString(card.m_Suit) << "')";

            oss << ", Flop: ";
            for (const int cardValue : context.m_Data.m_Flop)
            {
                pcmn::Card card;
                card.FromEvalFormat(cardValue);
                oss << "('" << pcmn::Card::ToString(card.m_Value) << "' of '" << pcmn::Suit::ToString(card.m_Suit) << "')";
            }

            LOG_TRACE("Win: [%s], bet: [%s], max bet: [%s], pot: [%s], bot stack: [%s], cards: [%s]") % winRate % player.Bet() % context.m_MaxBet % context.m_Pot % player.Stack() % oss.str();
            LOG_TRACE("\nDecision >>>>: [%s]\nDecision <<<<: [%s]") % params.ToString() % reply.ShortDebugString();
        }

	
		m_Connection.Send(reply);
	}
	CATCH_PASS_EXCEPTIONS("Failed to make a dicision");
}	

float DecisionMaker::GetPlayerWinRate(const pcmn::Player& bot, const pcmn::TableContext& context, const pcmn::Player::Queue& activePlayers) const
{
	SCOPED_LOG(m_Log);

	CHECK(bot.Cards().size() == 2, "Player cards info is invalid", bot.Cards().size());

	// prepare request
	IStatistics::PlayerInfo::List ranges;
	ranges.reserve(activePlayers.size());
	for (const pcmn::Player& player : activePlayers)
	{
        if (player.GetActions().empty() || player.Name() == bot.Name())
            continue;

		pcmn::Player::ActionDesc::List actions = player.GetActions().front();

        // remove useless actions
        actions.erase(std::remove_if(actions.begin(), actions.end(), [](const pcmn::Player::ActionDesc& a){ return a.m_Id > pcmn::Action::Raise; }), actions.end());

        if (actions.empty())
            continue;

		ranges.resize(ranges.size() + 1);
		IStatistics::PlayerInfo& current = ranges.back();

		// player name
		current.m_Name = player.Name();

        // actions
        current.m_Actions = actions;
	}

	// fetch statistics
	if (!ranges.empty())
		m_Stat.GetRanges(ranges);

	// fill card ranges
	std::vector<short> cardRanges;
     
	for (const IStatistics::PlayerInfo& info : ranges)
	{
        int range = info.m_CardRange;
        if (!range)
            continue;

        if (range < cfg::MIN_CARD_RANGE)
            range = cfg::MIN_CARD_RANGE;

        if (range > cfg::CARD_DECK_SIZE)
            range = cfg::CARD_DECK_SIZE;

        cardRanges.push_back(range);
	}

	unsigned size = activePlayers.size();
	if (size > cfg::MAX_EQUITY_PLAYERS)
		size = cfg::MAX_EQUITY_PLAYERS;

	if (cardRanges.size() > size)
		std::sort(cardRanges.begin(), cardRanges.end()); // remove bigger values(biggest value - worst hand)

    cardRanges.resize(size, cfg::CARD_DECK_SIZE);

    if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
    {
        std::ostringstream oss;
        std::copy(cardRanges.begin(), cardRanges.end(), std::ostream_iterator<int>(oss, " "));
        LOG_TRACE("Ranges: [%s]") % oss.str();
    }
	
	const float percents = m_Evaluator.GetEquity
	(
		bot.Cards()[0].ToEvalFormat(),
		bot.Cards()[1].ToEvalFormat(), 
		context.m_Data.m_Flop, 
		cardRanges
	);
	
	return percents / 100;
}

pcmn::Danger::Value DecisionMaker::GetDanger(const pcmn::Player& bot, const pcmn::Player::Queue& activePlayers, float botRate) const
{
	SCOPED_LOG(m_Log);

	// prepare request
	IStatistics::PlayerInfo::List equities;
	equities.reserve(activePlayers.size());
// 	for (const pcmn::Player& player : activePlayers)
// 	{
// 		pcmn::Player::Actions actions = player.GetActions();
// 
//         // remove useless actions
//         actions.erase(std::remove_if(actions.begin(), actions.end(), [](const pcmn::Player::ActionDesc& a){ return a.m_Action > pcmn::Action::Raise; }), actions.end());
// 
// 		if (actions.empty() || player.Name() == bot.Name())
// 			continue;
// 
// 		// only aggressive actions if we have much players
// 		if (activePlayers.size() > 2)
// 		{
// 			std::sort
// 			(
// 				actions.begin(), 
// 				actions.end(), 
// 				[](const pcmn::Player::ActionDesc& lhs, const pcmn::Player::ActionDesc& rhs)
// 				{
// 					return lhs.m_Action < rhs.m_Action;
// 				}
// 			);
// 
// 			if (actions.back().m_Action < pcmn::Action::Bet)
// 				continue;
// 		}
// 
// 		equities.resize(equities.size() + 1);
// 		IStatistics::PlayerInfo& current = equities.back();
// 
// 		// player name
// 		current.m_Name = player.Name();
// 		
// 		// copy all actions
// 		for (const pcmn::Player::ActionDesc& action : actions)
// 		{
// 			if (std::find(current.m_Actions.begin(), current.m_Actions.end(), action.m_Action) == current.m_Actions.end())
// 				current.m_Actions.push_back(action.m_Action);
// 		}
// 
// 		// get max pot range
// 		std::sort
// 		(
// 			actions.begin(), 
// 			actions.end(), 
// 			[](const pcmn::Player::ActionDesc& lhs, const pcmn::Player::ActionDesc& rhs)
// 			{
// 				return lhs.m_Value < rhs.m_Value;
// 			}
// 		);
// 
// 		current.m_Bet = actions.back().m_Value;
// 	}

    LOG_TRACE("Equities size: [%s]") % equities.size();

	if (equities.empty())
		return pcmn::Danger::Normal;

	// fetch statistics
	const unsigned count = m_Stat.GetEquities(equities);
    if (!count)
        return pcmn::Danger::Normal;

	// compare equities
	for (const IStatistics::PlayerInfo& equity : equities)
	{
		if (equity.m_WinRate > botRate)
			return pcmn::Danger::High;
	}

	// player list empty - unknown value(normal) - else all players have less than bot
	if ((activePlayers.size() - 1) / 2 > count)
		return pcmn::Danger::Normal;

	return pcmn::Danger::Low;
}

pcmn::Player::Style::Value DecisionMaker::GetBotAverageStyle(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers) const
{
	SCOPED_LOG(m_Log);

	// get any player
	const pcmn::Player::Queue::const_iterator it = std::find_if
	(
		activePlayers.begin(),
		activePlayers.end(),
		[&](const pcmn::Player& current)
		{
			return current.Name() != player.Name();
		}
	);

    // failed to find any players
	if (it == activePlayers.end())
        return pcmn::Player::Style::Normal;

	int checks = 0;
	int calls = 0;
	int raises = 0;
	m_Stat.GetLastActions(player.Name(), it->Name(), checks, calls, raises);

    if (raises >= 10)
        return pcmn::Player::Style::Aggressive;

	if (checks + calls + raises < 20)
		return pcmn::Player::Style::Normal;

	if (raises > 1)
		return pcmn::Player::Style::Normal;

	return pcmn::Player::Style::Passive;
}

pcmn::Player::Style::Value DecisionMaker::GetBotStyle(const pcmn::Player& bot) const
{
	SCOPED_LOG(m_Log);

	int calls = 0;
	for (const pcmn::Player::ActionDesc::List& actions : bot.GetActions())
	{
        for (const pcmn::Player::ActionDesc& action : actions)
        {
            if (action.m_Id == pcmn::Action::Bet || action.m_Id == pcmn::Action::Raise)
                return pcmn::Player::Style::Aggressive;
            else
            if (action.m_Id == pcmn::Action::Call)
                ++calls;
        }
	}

	// more than one call it's a normal style(one call from preflop), else - call and checks - it's passive style
	return (calls > 1) ? pcmn::Player::Style::Normal : pcmn::Player::Style::Passive;
}

unsigned DecisionMaker::GetMaxStack(const  pcmn::Player::Queue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	unsigned max = 0;

	for (const pcmn::Player& player : activePlayers)
	{
		if (player.Stack() > max)
			max = player.Stack();
	}
	return max;
}

const pcmn::Player& DecisionMaker::GetPlayer(const  pcmn::Player::Queue& activePlayers, const std::string& name) const
{
	const  pcmn::Player::Queue::const_iterator it = std::find_if
	(
		activePlayers.begin(),
		activePlayers.end(),
		[&](const pcmn::Player& player)
		{
			return player.Name() == name;
		}
	);

	CHECK(activePlayers.end() != it, "Failed to find player by name", name);
	return *it;
}

}