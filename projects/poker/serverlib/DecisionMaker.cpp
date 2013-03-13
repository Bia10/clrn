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
	
		neuro::Params params;
	
		// win rate
		params.m_WinRate = GetPlayerWinRate(player, context, activePlayers);
	
		// position 
		params.m_Position = position;
	
		// bet pot rate
		params.m_BetPotSize = pcmn::BetSize::FromValue(static_cast<float>(context.m_MaxBet - player.Bet()) / context.m_Pot, true);
	
		// bet stack rate
		params.m_BetStackSize = pcmn::BetSize::FromValue(player.Stack() ? static_cast<float>(context.m_MaxBet - player.Bet()) / player.Stack() : 0, false);
	
		// active players
		params.m_ActivePlayers = pcmn::Player::Count::FromValue(activePlayers.size() - 1);
	
		// most aggressive player
		params.m_MostAggressiveStyle = GetMostAggressiveStyle(activePlayers);
	
		// unusual style
		params.m_UnusualStyle = GetUnusualStyle(activePlayers);
	
		// bot play style
		params.m_BotStyle = GetBotStyle(activePlayers);
	
		// bot stack size
		params.m_BotStackSize = pcmn::StackSize::FromValue(player.Stack(), context.m_BigBlind, GetMaxStack(activePlayers));
	
		std::vector<float> in;
		std::vector<float> out;
		params.ToNeuroFormat(in, out);
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
			reply.set_amount(context.m_MaxBet * 3);
		}

		LOG_TRACE("Decision input: win: [%s], pos: [%s], pot: [%s], stack: [%s], players: [%s], aggression: [%s], unusual: [%s], bot style: [%s], bot stack: [%s]")
			% pcmn::WinRate::ToString(params.m_WinRate)
			% pcmn::Player::Position::ToString(params.m_Position)
			% pcmn::BetSize::ToString(params.m_BetPotSize)
			% pcmn::BetSize::ToString(params.m_BetStackSize)
			% pcmn::Player::Count::ToString(params.m_ActivePlayers)
			% pcmn::Player::Style::ToString(params.m_MostAggressiveStyle)
			% pcmn::Player::Style::ToString(params.m_UnusualStyle)
			% pcmn::Player::Style::ToString(params.m_BotStyle)
			% pcmn::StackSize::ToString(params.m_BotStackSize);

		LOG_TRACE("Decision output: [%s]") % reply.ShortDebugString();
	
		m_Connection.Send(reply);
	}
	CATCH_PASS_EXCEPTIONS("Failed to make a dicision");
}	

pcmn::WinRate::Value DecisionMaker::GetPlayerWinRate(const pcmn::Player& player, const pcmn::TableContext& context, const PlayerQueue& activePlayers) const
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

	return pcmn::WinRate::FromValue(percents);
}

pcmn::Player::Style::Value DecisionMaker::GetMostAggressiveStyle(const PlayerQueue& activePlayers) const
{
	SCOPED_LOG(m_Log);
	return pcmn::Player::Style::Normal;
}

pcmn::Player::Style::Value DecisionMaker::GetUnusualStyle(const PlayerQueue& activePlayers) const
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


}