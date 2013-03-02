#include "Teacher.h"
#include "Modules.h"
#include "Exception.h"
#include "FileSystem.h"
#include "../neuro/NetworkTeacher.h"
#include "../neuro/NeuroNetwork.h"
#include "Config.h"

#include "wx/msgdlg.h"

#include <fstream>
#include <sstream>

namespace tchr
{

Teacher::Teacher() 
	: TeacherMainFrame(NULL)
{
	m_StatusBar->SetStatusText("ready");
	SetGuiParams(m_CurrentParams);
	m_CurrentParams.m_CheckFold = true;
}

void Teacher::OnWinRate(wxCommandEvent& event)
{
	m_CurrentParams.m_WinRate = static_cast<pcmn::WinRate::Value>(event.GetInt());
}

void Teacher::OnPosition(wxCommandEvent& event)
{
	m_CurrentParams.m_Position = static_cast<pcmn::Player::Position::Value>(event.GetInt());
}

void Teacher::OnPotRatio(wxCommandEvent& event)
{
	m_CurrentParams.m_BetPotSize = static_cast<pcmn::BetSize::Value>(event.GetInt());
}

void Teacher::OnStackRatio(wxCommandEvent& event)
{
	m_CurrentParams.m_BetStackSize = static_cast<pcmn::BetSize::Value>(event.GetInt());
}

void Teacher::OnPlayers(wxCommandEvent& event)
{
	m_CurrentParams.m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(event.GetInt());
}

void Teacher::OnPlayersStyle(wxCommandEvent& event)
{
	m_CurrentParams.m_MostAggressiveStyle = static_cast<pcmn::Player::Style::Value>(event.GetInt());
}

void Teacher::OnStyleChanges(wxCommandEvent& event)
{
	m_CurrentParams.m_UnusualStyle = static_cast<pcmn::Player::Style::Value>(event.GetInt());
}

void Teacher::OnBotStyle(wxCommandEvent& event)
{
	m_CurrentParams.m_BotStyle = static_cast<pcmn::Player::Style::Value>(event.GetInt());
}

void Teacher::OnBotStackSize(wxCommandEvent& event)
{
	m_CurrentParams.m_BotStackSize = static_cast<pcmn::StackSize::Value>(event.GetInt());
}

void Teacher::OnAction(wxCommandEvent& event)
{
	m_CurrentParams.m_CheckFold = false;
	m_CurrentParams.m_BetCall = false;
	m_CurrentParams.m_RaiseReraise = false;

	switch (event.GetSelection())
	{
	case 0: m_CurrentParams.m_CheckFold = true; break;
	case 1: m_CurrentParams.m_BetCall = true; break;
	case 2: m_CurrentParams.m_RaiseReraise = true; break;
	}
}

void Teacher::OnSave(wxCommandEvent& event)
{
	try
	{
		AddParameters();

		const std::string path = fs::FullPath(cfg::DATA_FILE_NAME);
		std::ofstream file(path.c_str(), std::ios_base::out);
		CHECK(file.is_open(), path);

		for (const neuro::Params& params : m_Parameters)
			params.Serialize(file);

		std::ostringstream oss;
		oss << "saved " << m_Parameters.size() << " params to " << path;
		m_StatusBar->SetStatusText(oss.str());
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
	}
}

void Teacher::OnNext(wxCommandEvent& event)
{
	try
	{
		AddParameters();
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
		return;
	}

	// bot stack
	if (m_BotStackCheck->IsChecked())
	{
		if (m_CurrentParams.m_BotStackSize == pcmn::StackSize::Max)
		{
			m_CurrentParams.m_BotStackSize = pcmn::StackSize::Small;
			m_BostStackSizeChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BotStackSize = static_cast<pcmn::StackSize::Value>(int(m_CurrentParams.m_BotStackSize) + 1);
			m_BostStackSizeChoice->SetSelection(m_BostStackSizeChoice->GetSelection() + 1);
			return;
		}
	}
	
	// bot style
	if (m_BotStyleCheck->IsChecked())
	{
		if (m_CurrentParams.m_BotStyle == pcmn::Player::Style::Max)
		{
			m_CurrentParams.m_BotStyle = pcmn::Player::Style::Passive;
			m_BotStyleChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BotStyle = static_cast<pcmn::Player::Style::Value>(int(m_CurrentParams.m_BotStyle) + 1);
			m_BotStyleChoice->SetSelection(m_BotStyleChoice->GetSelection() + 1);
			return;
		}
	}

	// players unusual style
	if (m_StyleChengeCheck->IsChecked())
	{
		if (m_CurrentParams.m_UnusualStyle == pcmn::Player::Style::Max)
		{
			m_CurrentParams.m_UnusualStyle = pcmn::Player::Style::Passive;
			m_StyleChangeChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_UnusualStyle = static_cast<pcmn::Player::Style::Value>(int(m_CurrentParams.m_UnusualStyle) + 1);
			m_StyleChangeChoice->SetSelection(m_StyleChangeChoice->GetSelection() + 1);
			return;
		}
	}

	// most aggressive style
	if (m_PlayersStyleCheck->IsChecked())
	{
		if (m_CurrentParams.m_MostAggressiveStyle == pcmn::Player::Style::Max)
		{
			m_CurrentParams.m_MostAggressiveStyle = pcmn::Player::Style::Passive;
			m_PlayersStyleChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_MostAggressiveStyle = static_cast<pcmn::Player::Style::Value>(int(m_CurrentParams.m_MostAggressiveStyle) + 1);
			m_PlayersStyleChoice->SetSelection(m_PlayersStyleChoice->GetSelection() + 1);
			return;
		}
	}

	// active players count
	if (m_PlayersCheck->IsChecked())
	{
		if (m_CurrentParams.m_ActivePlayers == pcmn::Player::Count::Max)
		{
			m_CurrentParams.m_ActivePlayers = pcmn::Player::Count::One;
			m_PlayersChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(int(m_CurrentParams.m_ActivePlayers) + 1);
			m_PlayersChoice->SetSelection(m_PlayersChoice->GetSelection() + 1);
			return;
		}
	}

	// bet stack size
	if (m_StackRatioCheck->IsChecked())
	{
		if (m_CurrentParams.m_BetStackSize == pcmn::BetSize::Max)
		{
			m_CurrentParams.m_BetStackSize = pcmn::BetSize::VeryLow;
			m_StackRateChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BetStackSize = static_cast<pcmn::BetSize::Value>(int(m_CurrentParams.m_BetStackSize) + 1);
			m_StackRateChoice->SetSelection(m_StackRateChoice->GetSelection() + 1);
			return;
		}
	}

	// bet pot size
	if (m_PotRatioCheck->IsChecked())
	{
		if (m_CurrentParams.m_BetPotSize == pcmn::BetSize::Max)
		{
			m_CurrentParams.m_BetPotSize = pcmn::BetSize::VeryLow;
			m_PotRateChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BetPotSize = static_cast<pcmn::BetSize::Value>(int(m_CurrentParams.m_BetPotSize) + 1);
			m_PotRateChoice->SetSelection(m_PotRateChoice->GetSelection() + 1);
			return;
		}
	}

	// bot position
	if (m_PositionCheck->IsChecked())
	{
		if (m_CurrentParams.m_Position == pcmn::Player::Position::Max)
		{
			m_CurrentParams.m_Position = pcmn::Player::Position::Early;
			m_PositionChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_Position = static_cast<pcmn::Player::Position::Value>(int(m_CurrentParams.m_Position) + 1);
			m_PositionChoice->SetSelection(m_PositionChoice->GetSelection() + 1);
			return;
		}
	}

	// win rate
	if (m_WinRateCheck->IsChecked())
	{
		if (m_CurrentParams.m_WinRate == pcmn::WinRate::Max)
		{
			m_CurrentParams.m_WinRate = pcmn::WinRate::VeryLow;
			m_WinRateChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_WinRate = static_cast<pcmn::WinRate::Value>(int(m_CurrentParams.m_WinRate) + 1);
			m_WinRateChoice->SetSelection(m_WinRateChoice->GetSelection() + 1);
			return;
		}
	}
}

void Teacher::OnLoad(wxCommandEvent& event)
{
	try
	{
		const std::string path = fs::FullPath(cfg::DATA_FILE_NAME);
		std::ifstream file(path.c_str(), std::ios_base::in);
		CHECK(file.is_open(), path);

		m_Parameters.clear();

		std::vector<int> data;
		std::copy(std::istream_iterator<int>(file), std::istream_iterator<int>(), std::back_inserter(data));

		CHECK(!(data.size() % 12), data.size());

		m_Parameters.resize(data.size() / 12);

		std::vector<int>::const_iterator it = data.begin();
		const std::vector<int>::const_iterator itEnd = data.end();
		std::size_t counter = 0;
		for (; it != itEnd; ++counter)
			it = m_Parameters[counter].Deserialize(it);

		if (!m_Parameters.empty())
		{
			m_CurrentParams = m_Parameters.back();

			SetGuiParams(m_CurrentParams);
		}

		std::ostringstream oss;
		oss << "loaded " << m_Parameters.size() << " params from " << path;
		m_StatusBar->SetStatusText(oss.str());
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
	}
}

void Teacher::OnTeach(wxCommandEvent& event)
{
	neuro::NetworkTeacher teacher(cfg::NETWORK_FILE_NAME);

	std::vector<float> in;
	std::vector<float> out;
	for (std::size_t i = 0; i < cfg::TEACH_REPETITIONS_COUNT; ++i)
	{
		for (const neuro::Params& params : m_Parameters)
		{
			params.ToNeuroFormat(in, out);
			teacher.Process(in, out);
		}
	}

	m_StatusBar->SetStatusText("trained");
}

void Teacher::OnTest(wxCommandEvent& event)
{
	neuro::Network net(cfg::NETWORK_FILE_NAME);

	std::vector<float> in;
	std::vector<float> out;

	m_CurrentParams.ToNeuroFormat(in, out);
	net.Process(in, out);

	m_CurrentParams.m_CheckFold = false;
	m_CurrentParams.m_BetCall = false;
	m_CurrentParams.m_RaiseReraise = false;

	if (out[0] > out[1] && out[0] > out[2])
		m_CurrentParams.m_CheckFold = true;
	else
	if (out[1] > out[0] && out[1] > out[2])
		m_CurrentParams.m_BetCall = true;
	else
	if (out[2] > out[0] && out[2] > out[1])
		m_CurrentParams.m_RaiseReraise = true;

	SetGuiParams(m_CurrentParams);
	m_StatusBar->SetStatusText("tested");
}

void Teacher::AddParameters()
{
	std::size_t counter = 0;
	if (m_CurrentParams.m_CheckFold)
		++counter;
	if (m_CurrentParams.m_BetCall)
		++counter;
	if (m_CurrentParams.m_RaiseReraise)
		++counter;

	CHECK(counter == 1, "Only one output parameter must be set", counter);

	if (!m_Parameters.empty() && !memcmp(&m_Parameters.back(), &m_CurrentParams, sizeof(neuro::Params)))
	{
		std::ostringstream oss;
		oss << "parameters are same, save skipped";
		m_StatusBar->SetStatusText(oss.str());
		return;
	}

	m_Parameters.push_back(m_CurrentParams);

	std::ostringstream oss;
	oss << "parameter " << m_Parameters.size() << " added";
	m_StatusBar->SetStatusText(oss.str());
}

void Teacher::SetGuiParams(const neuro::Params& params)
{
	m_WinRateChoice->SetSelection(params.m_WinRate);
	m_PositionChoice->SetSelection(params.m_Position);
	m_PotRateChoice->SetSelection(params.m_BetPotSize);
	m_StackRateChoice->SetSelection(params.m_BetStackSize);
	m_PlayersChoice->SetSelection(params.m_ActivePlayers);
	m_PlayersStyleChoice->SetSelection(params.m_MostAggressiveStyle);
	m_StyleChangeChoice->SetSelection(params.m_UnusualStyle);
	m_BotStyleChoice->SetSelection(params.m_BotStyle);
	m_BostStackSizeChoice->SetSelection(params.m_BotStackSize);

	int action = 0;
	if (params.m_BetCall)
		action = 1;
	else
	if (params.m_RaiseReraise)
		action = 2;

	m_ActionRadio->SetSelection(action);
}

}