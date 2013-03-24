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

namespace srv
{

const int CURRENT_MODULE_ID = Modules::Server;

DecisionMaker::DecisionMaker
(
	ILog& logger, 
	const pcmn::Evaluator& evaluator, 
	const srv::IStatistics& stats, 
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

void DecisionMaker::MakeDecision(const pcmn::Player& player, const PlayerQueue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position)
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
	
		// bet pot rate
		float potRate;
		params.m_BetPotSize = pcmn::BetSize::FromPot(player.Bet(), context.m_MaxBet, context.m_Pot, potRate);
		in.push_back(potRate);
	
		// bet stack rate
		float stackRate;
		params.m_BetStackSize = pcmn::BetSize::FromStack(player.Bet(), context.m_MaxBet, player.Stack(), stackRate);
		in.push_back(stackRate);
	
		// active players
		params.m_ActivePlayers = pcmn::Player::Count::FromValue(activePlayers.size() - 1);
		in.push_back(static_cast<float>(params.m_ActivePlayers) / pcmn::Player::Count::Max);
	
		// danger
		params.m_Danger = GetDanger(player, activePlayers, winRate);
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

			unsigned amount = context.m_MaxBet ? context.m_MaxBet * 3 : context.m_BigBlind * 5;
			amount = (amount / 10) * 10;

			if (amount > player.Stack() / 2)
				amount = player.Stack();

			reply.set_amount(amount);
		}

		LOG_TRACE("Decision input: win: [%s]:[%s], pos: [%s], pot: [%s], stack: [%s], players: [%s], danger: [%s], bot avg style: [%s], bot style: [%s], bot stack: [%s]")
			% pcmn::WinRate::ToString(params.m_WinRate)
			% int(winRate * 100)
			% pcmn::Player::Position::ToString(params.m_Position)
			% pcmn::BetSize::ToString(params.m_BetPotSize)
			% pcmn::BetSize::ToString(params.m_BetStackSize)
			% pcmn::Player::Count::ToString(params.m_ActivePlayers)
			% pcmn::Danger::ToString(params.m_Danger)
			% pcmn::Player::Style::ToString(params.m_BotAverageStyle)
			% pcmn::Player::Style::ToString(params.m_BotStyle)
			% pcmn::StackSize::ToString(params.m_BotStackSize);

		LOG_TRACE("Decision output: [%s]") % reply.ShortDebugString();
	
		m_Connection.Send(reply);
	}
	CATCH_PASS_EXCEPTIONS("Failed to make a dicision");
}	

float DecisionMaker::GetPlayerWinRate(const pcmn::Player& bot, const pcmn::TableContext& context, const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);

	CHECK(bot.Cards().size() == 2, "Player cards info is invalid", bot.Cards().size());

	// prepare request
	IStatistics::PlayerInfo::List ranges;
	ranges.reserve(activePlayers.size());
	for (const pcmn::Player::Ptr& player : activePlayers)
	{
		pcmn::Player::Actions actions = player->GetActions();
		if (actions.empty() || player->Name() == bot.Name())
			continue;

		ranges.resize(ranges.size() + 1);
		IStatistics::PlayerInfo& current = ranges.back();

		// player name
		current.m_Name = player->Name();

		// find max action
		std::sort
		(
			actions.begin(), 
			actions.end(), 
			[](const pcmn::Player::ActionDesc& lhs, const pcmn::Player::ActionDesc& rhs)
			{
				return lhs.m_Action < rhs.m_Action;
			}
		);

		current.m_Actions.push_back(actions.back().m_Action);

		// get max pot range
		std::sort
		(
			actions.begin(), 
			actions.end(), 
			[](const pcmn::Player::ActionDesc& lhs, const pcmn::Player::ActionDesc& rhs)
			{
				return lhs.m_PotAmount < rhs.m_PotAmount;
			}
		);

		current.m_PotAmount = actions.back().m_PotAmount;
	}

	// fetch statistics
	if (!ranges.empty())
		m_Stat.GetRanges(ranges);

	// fill card ranges
	std::vector<short> cardRanges;

	for (const IStatistics::PlayerInfo& info : ranges)
	{
		if (info.m_CardRange)
			cardRanges.push_back(info.m_CardRange);
	}

	unsigned size = activePlayers.size();
	if (size > cfg::MAX_EQUITY_PLAYERS)
		size = cfg::MAX_EQUITY_PLAYERS;

	if (cardRanges.size() > size)
	{
		// remove bigger values(biggest value - worst hand)
		std::sort(cardRanges.begin(), cardRanges.end());
		cardRanges.resize(size);
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

pcmn::Danger::Value DecisionMaker::GetDanger(const pcmn::Player& bot, const PlayerQueue& activePlayers, float botRate) const
{
	SCOPED_LOG(m_Log);

	// prepare request
	IStatistics::PlayerInfo::List equities;
	equities.reserve(activePlayers.size());
	for (const pcmn::Player::Ptr& player : activePlayers)
	{
		pcmn::Player::Actions actions = player->GetActions();
		if (actions.empty() || player->Name() == bot.Name())
			continue;

		equities.resize(equities.size() + 1);
		IStatistics::PlayerInfo& current = equities.back();

		// player name
		current.m_Name = player->Name();
		
		// copy all actions
		for (const pcmn::Player::ActionDesc& action : actions)
		{
			if (std::find(current.m_Actions.begin(), current.m_Actions.end(), action.m_Action) == current.m_Actions.end())
				current.m_Actions.push_back(action.m_Action);
		}

		// get max pot range
		std::sort
		(
			actions.begin(), 
			actions.end(), 
			[](const pcmn::Player::ActionDesc& lhs, const pcmn::Player::ActionDesc& rhs)
			{
				return lhs.m_PotAmount < rhs.m_PotAmount;
			}
		);

		current.m_PotAmount = actions.back().m_PotAmount;
	}

	// fetch statistics
	const unsigned count = m_Stat.GetEquities(equities);

	// player list empty - unknown value(normal) - else all players have less than bot
	if (!count)
		return pcmn::Danger::Normal;

	// compare equities
	for (const IStatistics::PlayerInfo& equity : equities)
	{
		if (equity.m_WinRate > botRate)
			return pcmn::Danger::High;
	}

	return pcmn::Danger::Low;
}

pcmn::Player::Style::Value DecisionMaker::GetBotAverageStyle(const pcmn::Player& player, const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);

	// get any player
	const PlayerQueue::const_iterator it = std::find_if
	(
		activePlayers.begin(),
		activePlayers.end(),
		[&](const pcmn::Player::Ptr& current)
		{
			return current->Name() != player.Name();
		}
	);

	CHECK(it != activePlayers.end(), "Failed to find any another player");

	int checks = 0;
	int calls = 0;
	int raises = 0;
	m_Stat.GetLastActions(player.Name(), (*it)->Name(), checks, calls, raises);

	if (checks + calls + raises < 20)
		return pcmn::Player::Style::Normal;

	if (raises > 10)
		return pcmn::Player::Style::Aggressive;

	if (raises > 1)
		return pcmn::Player::Style::Normal;

	return pcmn::Player::Style::Passive;
}

pcmn::Player::Style::Value DecisionMaker::GetBotStyle(const pcmn::Player& bot) const
{
	SCOPED_LOG(m_Log);

	int calls = 0;
	for (const pcmn::Player::ActionDesc& action : bot.GetActions())
	{
		if (action.m_Action == pcmn::Action::Bet || action.m_Action == pcmn::Action::Raise)
			return pcmn::Player::Style::Aggressive;
		else
		if (action.m_Action == pcmn::Action::Call)
			++calls;
	}

	// more than one call it's a normal style(one call from preflop), else - call and checks - it's passive style
	return (calls > 1) ? pcmn::Player::Style::Normal : pcmn::Player::Style::Passive;
}

unsigned DecisionMaker::GetMaxStack(const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	unsigned max = 0;

	for (const pcmn::Player::Ptr& player : activePlayers)
	{
		if (player->Stack() > max)
			max = player->Stack();
	}
	return max;
}

const pcmn::Player& DecisionMaker::GetPlayer(const PlayerQueue& activePlayers, const std::string& name) const
{
	const PlayerQueue::const_iterator it = std::find_if
	(
		activePlayers.begin(),
		activePlayers.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player->Name() == name;
		}
	);

	CHECK(activePlayers.end() != it, "Failed to find player by name", name);
	return **it;
}

}