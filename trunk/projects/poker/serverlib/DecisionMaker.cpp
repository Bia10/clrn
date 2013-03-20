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
		params.m_Danger = GetDanger(activePlayers);
		in.push_back(static_cast<float>(params.m_Danger) / pcmn::Player::Style::Max);
	
		// bot average style
		params.m_BotAverageStyle = GetBotAverageStyle(activePlayers);
		in.push_back(static_cast<float>(params.m_BotAverageStyle) / pcmn::Player::Style::Max);
	
		// bot play style
		params.m_BotStyle = GetBotStyle(activePlayers);
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
				reply.set_action(pcmn::Action::Call);
				reply.set_amount(context.m_MaxBet - player.Bet());
			}
			else
			{
				reply.set_action(pcmn::Action::Bet);
				reply.set_amount(context.m_BigBlind * 4);
			}
		}
		else
		if (out[2] > out[0] && out[2] > out[1])
		{
			reply.set_action(pcmn::Action::Raise);
			unsigned amount = context.m_MaxBet ? context.m_MaxBet * 3 : context.m_BigBlind * 5;
			if (amount > player.Stack() * 3 / 2)
				amount = player.Stack();

			reply.set_amount(amount);
		}

		LOG_TRACE("Decision input: win: [%s]:[%s], pos: [%s], pot: [%s], stack: [%s], players: [%s], aggression: [%s], unusual: [%s], bot style: [%s], bot stack: [%s]")
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

float DecisionMaker::GetPlayerWinRate(const pcmn::Player& player, const pcmn::TableContext& context, const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);

	CHECK(player.Cards().size() == 2, "Player cards info is invalid", player.Cards().size());

	int players = activePlayers.size();
	if (players > cfg::MAX_EQUITY_PLAYERS)
		players = cfg::MAX_EQUITY_PLAYERS;

	const std::vector<short> ranges(cfg::MAX_EQUITY_PLAYERS, cfg::CARD_DECK_SIZE);
	const float percents = m_Evaluator.GetEquity
	(
		player.Cards()[0].ToEvalFormat(),
		player.Cards()[1].ToEvalFormat(), 
		context.m_Data.m_Flop, 
		ranges
	);

	return percents / 100;
}

pcmn::Danger::Value DecisionMaker::GetDanger(const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	return pcmn::Danger::Normal;
}

pcmn::Player::Style::Value DecisionMaker::GetBotAverageStyle(const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	return pcmn::Player::Style::Normal;
}

pcmn::Player::Style::Value DecisionMaker::GetBotStyle(const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	return pcmn::Player::Style::Normal;
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

float DecisionMaker::GetDanger() const
{
	return 0;
}

}