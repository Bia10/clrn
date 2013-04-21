#include "Teacher.h"
#include "Modules.h"
#include "Exception.h"
#include "FileSystem.h"
#include "../neuro/NetworkTeacher.h"
#include "../neuro/NeuroNetwork.h"
#include "../neuro/DatabaseReader.h"
#include "../neuro/DatabaseWriter.h"
#include "Config.h"
#include "Log.h"
#include "SQLiteDB.h"

#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/filedlg.h>

#include <fstream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

namespace tchr
{

Teacher::Teacher() 
	: TeacherMainFrame(NULL)
    , m_All(false)
    , m_CurrentRow(0)
{
	m_StatusBar->SetStatusText("ready");
	SetGuiParams(m_CurrentParams);
	m_CurrentParams.m_CheckFold = true;
	m_Gauge->Hide(); 

    const unsigned paramsSize = neuro::Params::MaxHash();
    m_Parameters.resize(paramsSize);
    SetPramsValues();
    UpdateGrid(m_CurrentRow);
}

void Teacher::OnWinRate(wxCommandEvent& event)
{
	m_CurrentParams.m_WinRate = static_cast<pcmn::WinRate::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnPosition(wxCommandEvent& event)
{
	m_CurrentParams.m_Position = static_cast<pcmn::Player::Position::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnPotRatio(wxCommandEvent& event)
{
	m_CurrentParams.m_BetSize = static_cast<pcmn::BetSize::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnPlayers(wxCommandEvent& event)
{
	m_CurrentParams.m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnPlayersStyle(wxCommandEvent& event)
{
	m_CurrentParams.m_Danger = static_cast<pcmn::Danger::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnStyleChanges(wxCommandEvent& event)
{
	m_CurrentParams.m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnBotStyle(wxCommandEvent& event)
{
	m_CurrentParams.m_BotStyle = static_cast<pcmn::Player::Style::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnBotStackSize(wxCommandEvent& event)
{
	m_CurrentParams.m_BotStackSize = static_cast<pcmn::StackSize::Value>(event.GetInt());
    SetRowViewToCurrentParams();
}

void Teacher::OnAction(wxCommandEvent& event)
{
	m_CurrentParams.m_CheckFold = false;
	m_CurrentParams.m_CheckCall = false;
	m_CurrentParams.m_BetRaise = false;

	switch (event.GetSelection())
	{
	case 0: m_CurrentParams.m_CheckFold = true; break;
	case 1: m_CurrentParams.m_CheckCall = true; break;
	case 2: m_CurrentParams.m_BetRaise = true; break;
	}
}

void Teacher::OnSave(wxCommandEvent& event)
{
	try
	{
        wxFileDialog dialog
        (
            NULL, 
            "Choose a file to save", 
            wxEmptyString, 
            wxEmptyString, 
            "Data files (*.txt)|*.txt",
            wxFD_SAVE
        );

        dialog.ShowModal();
        const wxString filePath = dialog.GetPath();
        const std::string path = fs::FullPath(filePath.ToStdString());

        if (path.empty())
            return;

		neuro::Params::List oldParams;
		LoadParams(oldParams, path);

        m_All = false;
		MergeParams(oldParams, m_Parameters);
        m_All = false;
		m_Parameters = oldParams;

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
	IncrementChecked();
}

void Teacher::OnLoad(wxCommandEvent& event)
{
	try
	{
        wxFileDialog dialog
        (
            NULL, 
            "Choose a file to save", 
            wxEmptyString, 
            wxEmptyString, 
            "Data files (*.txt)|*.txt",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST
        );

        dialog.ShowModal();
        const wxString filePath = dialog.GetPath();
        const std::string path = fs::FullPath(filePath.ToStdString());

        if (path.empty())
            return;

		LoadParams(m_Parameters, path);

		if (!m_Parameters.empty())
		{
			m_CurrentParams = m_Parameters.front();
			SetGuiParams(m_CurrentParams);
            UpdateGrid(m_CurrentRow);
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
    boost::thread(boost::bind(&Teacher::TeachThread, this));
}

void Teacher::OnTest(wxCommandEvent& event)
{
	neuro::Network net(cfg::NETWORK_FILE_NAME);

	std::vector<float> in;
	std::vector<float> out;

	m_CurrentParams.ToNeuroFormat(in, out);
	net.Process(in, out);

	m_CurrentParams.m_CheckFold = false;
	m_CurrentParams.m_CheckCall = false;
	m_CurrentParams.m_BetRaise = false;

	if (out[0] > out[1] && out[0] > out[2])
		m_CurrentParams.m_CheckFold = true;
	else
	if (out[1] > out[0] && out[1] > out[2])
		m_CurrentParams.m_CheckCall = true;
	else
	if (out[2] > out[0] && out[2] > out[1])
		m_CurrentParams.m_BetRaise = true;

	SetGuiParams(m_CurrentParams);
	m_StatusBar->SetStatusText("tested");
}

void Teacher::AddParameters()
{
	std::size_t counter = 0;
	if (m_CurrentParams.m_CheckFold)
		++counter;
	if (m_CurrentParams.m_CheckCall)
		++counter;
	if (m_CurrentParams.m_BetRaise)
		++counter;

	CHECK(counter == 1, "Only one output parameter must be set", counter);

	const neuro::Params::List previous = m_Parameters;

	neuro::Params::List temp;
	temp.push_back(m_CurrentParams);
	MergeParams(m_Parameters, temp);

	std::ostringstream oss;
	oss << "parameter " << m_Parameters.size() << 
		(
			previous == m_Parameters ? " skipped" : 
			(
				previous.size() == m_Parameters.size() ? " replaced" : " added"
			)
		);
	m_StatusBar->SetStatusText(oss.str());
}

void Teacher::SetGuiParams(const neuro::Params& params)
{
	m_WinRateChoice->SetSelection(params.m_WinRate);
	m_PositionChoice->SetSelection(params.m_Position);
	m_BetSizeChoice->SetSelection(params.m_BetSize);
	m_PlayersChoice->SetSelection(params.m_ActivePlayers);
	m_DangerChoice->SetSelection(params.m_Danger);
	m_BotAverageStyleChoice->SetSelection(params.m_BotAverageStyle);
	m_BotStyleChoice->SetSelection(params.m_BotStyle);
	m_BostStackSizeChoice->SetSelection(params.m_BotStackSize);

	int action = 0;
	if (params.m_CheckCall)
		action = 1;
	else
	if (params.m_BetRaise)
		action = 2;

	m_ActionRadio->SetSelection(action);
}

void Teacher::IncrementChecked()
{
	try
	{
        m_All = false;
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
	if (m_BotAverageStyleCheck->IsChecked())
	{
		if (m_CurrentParams.m_BotAverageStyle == pcmn::Player::Style::Max)
		{
			m_CurrentParams.m_BotAverageStyle = pcmn::Player::Style::Passive;
			m_BotAverageStyleChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(int(m_CurrentParams.m_BotAverageStyle) + 1);
			m_BotAverageStyleChoice->SetSelection(m_BotAverageStyleChoice->GetSelection() + 1);
			return;
		}
	}

	// most aggressive style
	if (m_DangerCheck->IsChecked())
	{
		if (m_CurrentParams.m_Danger == pcmn::Danger::Max)
		{
			m_CurrentParams.m_Danger = pcmn::Danger::Low;
			m_DangerChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_Danger = static_cast<pcmn::Danger::Value>(int(m_CurrentParams.m_Danger) + 1);
			m_DangerChoice->SetSelection(m_DangerChoice->GetSelection() + 1);
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

	// bet size
	if (m_BetSizeCheck->IsChecked())
	{
		if (m_CurrentParams.m_BetSize == pcmn::BetSize::Max)
		{
			m_CurrentParams.m_BetSize = pcmn::BetSize::VeryLow;
			m_BetSizeChoice->SetSelection(0);
		}
		else
		{
			m_CurrentParams.m_BetSize = static_cast<pcmn::BetSize::Value>(int(m_CurrentParams.m_BetSize) + 1);
			m_BetSizeChoice->SetSelection(m_BetSizeChoice->GetSelection() + 1);
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

void Teacher::OnRange(wxCommandEvent& event)
{
	int totalCount = 1;

	// bot stack
	if (m_BotStackCheck->IsChecked())
	{
		totalCount *= m_BostStackSizeChoice->GetCount() - m_BostStackSizeChoice->GetSelection();
	}

	// bot style
	if (m_BotStyleCheck->IsChecked())
	{
		totalCount *= m_BotStyleChoice->GetCount() - m_BotStyleChoice->GetSelection();
	}

	// players unusual style
	if (m_BotAverageStyleCheck->IsChecked())
	{
		totalCount *= m_BotAverageStyleChoice->GetCount() - m_BotAverageStyleChoice->GetSelection();
	}

	// most aggressive style
	if (m_DangerCheck->IsChecked())
	{
		totalCount *= m_DangerChoice->GetCount() - m_DangerChoice->GetSelection();
	}

	// active players count
	if (m_PlayersCheck->IsChecked())
	{
		totalCount *= m_PlayersChoice->GetCount() - m_PlayersChoice->GetSelection();
	}

	// bet size
	if (m_BetSizeCheck->IsChecked())
	{
		totalCount *= m_BetSizeChoice->GetCount() - m_BetSizeChoice->GetSelection();
	}

	// bot position
	if (m_PositionCheck->IsChecked())
	{
		totalCount *= m_PositionChoice->GetCount() - m_PositionChoice->GetSelection();
	}

	// win rate
	if (m_WinRateCheck->IsChecked())
	{
		totalCount *= m_WinRateChoice->GetCount() - m_WinRateChoice->GetSelection();
	}

    const unsigned start = m_CurrentParams.Hash();
    neuro::Params::List paramsCopy(m_Parameters.begin() + start, m_Parameters.begin() + start + totalCount);
    for (neuro::Params& params : paramsCopy)
    {
        params.m_CheckFold = m_CurrentParams.m_CheckFold;
        params.m_CheckCall = m_CurrentParams.m_CheckCall;
        params.m_BetRaise = m_CurrentParams.m_BetRaise;
    }

    m_All = false;
    MergeParams(m_Parameters, paramsCopy);
    m_All = false;
    UpdateGrid(start + totalCount);
}

void Teacher::LoadParams(neuro::Params::List& params, const std::string& filePath)
{
    params.clear();
    params.resize(neuro::Params::MaxHash());

	const std::string path = fs::FullPath(filePath);
	if (!fs::Exists(path))
		return;

	std::ifstream file(path.c_str(), std::ios_base::in);
	CHECK(file.is_open(), path);

	std::vector<int> data;
	std::copy(std::istream_iterator<int>(file), std::istream_iterator<int>(), std::back_inserter(data));

	CHECK(!(data.size() % 12), data.size());

	neuro::Params::List tempParams(data.size() / 12);

	std::vector<int>::const_iterator it = data.begin();
	const std::vector<int>::const_iterator itEnd = data.end();
	std::size_t counter = 0;
	for (; it != itEnd; ++counter)
		it = tempParams[counter].Deserialize(it);

    for (const neuro::Params& params : tempParams)
        m_Parameters[params.Hash()] = params;
}

void Teacher::MergeParams(neuro::Params::List& dst, const neuro::Params::List& src)
{
	if (src.size() > 10)
		m_Gauge->Show();

	int count = 0;

	for (const neuro::Params& params : src)
	{
		if (src.size() > 10)
			m_Gauge->SetValue((count++ * 100) / src.size());

        const unsigned hash = params.Hash();

        neuro::Params& dstParams = dst[hash];

		int result = 0;

        if (m_All)
            result = 0;
        else
		if (!dstParams.IsDecisionEquals(params))
		{
			std::ostringstream oss;
			oss 
				<< "Exists: " << std::endl << dstParams.ToString() << std::endl << std::endl
				<< "Replacing with: " << std::endl << params.ToString() << std::endl << std::endl
				<< "Replace ?";

			static const wxString choices[] = {"Yes", "No", "All"};
			result = wxGetSingleChoiceIndex(oss.str().c_str(), "Already exists", _countof(choices), choices, 0);
		}

		if (result == 0)
			dstParams = params;
		else
		if (result == -1)
			return;
		else
		if (result == 2)
		{
			m_All = true;
            dstParams = params;
		}
		if (result == 1)
			continue;
	}

	if (src.size() > 10)
		m_Gauge->Hide();
}

void Teacher::RangeThread(int count)
{
	m_Gauge->Show();
	for (int i = 0 ; i < count; ++i)
	{
		IncrementChecked();
		m_Gauge->SetValue((i * 100) / count);
	}

	m_Gauge->Hide();
    m_All = false;
}

void Teacher::TeachThread()
{
    m_Gauge->Show();

    m_StatusBar->SetStatusText("preparing file...");

    const std::string filePath = fs::FullPath("train.temp.txt");
    std::ofstream ofs(filePath.c_str(), std::ios::binary | std::ios::out);
    CHECK(ofs.is_open(), filePath);
    fs::ScopedFile guard(filePath);

    /*
    The first line consists of three numbers: 
    The first is the number of training pairs in the file, the second is the number of inputs and the third is the number of outputs.  
    The rest of the file is the actual training data, consisting of one line with inputs, one with outputs etc.
    */

    ofs << m_Parameters.size() << " " << cfg::INPUT_COUNT << " " << cfg::OUTPUT_COUNT << std::endl;

    std::vector<float> in;
    std::vector<float> out;
    for (const neuro::Params& params : m_Parameters)
    {
        params.ToNeuroFormat(in, out);
        
        std::copy(in.begin(), in.end(), std::ostream_iterator<float>(ofs, " "));
        ofs << std::endl;

        std::copy(out.begin(), out.end(), std::ostream_iterator<float>(ofs, " "));
        ofs << std::endl;
    }

    ofs.close();

    const boost::posix_time::ptime start = boost::posix_time::microsec_clock().local_time();

    neuro::NetworkTeacher teacher(cfg::NETWORK_FILE_NAME);
    teacher.TrainOnFile(filePath, boost::bind(&Teacher::TrainCallback, this, _1, _2, _3, _4, start));

    m_Gauge->Hide();
    m_StatusBar->SetStatusText("trained");
}

void Teacher::UpdateGrid(unsigned paramsRow)
{
    const unsigned max = m_Grid->GetRows();

    if (paramsRow > max / 2)
    {
        paramsRow -= max / 2;
        m_Grid->SelectRow(max / 2);
    }
    else
    {
        m_Grid->SelectRow(paramsRow);
        paramsRow = 0;
    }

    if (m_Parameters.size() - paramsRow <= max)
        paramsRow = m_Parameters.size() - max;

    for (unsigned row = 0; row < max; ++row, ++paramsRow)
    {       
        const neuro::Params& params = m_Parameters[paramsRow];
        
        unsigned c = 0;
        m_Grid->SetCellValue(row, c++, pcmn::WinRate::ToString(params.m_WinRate));
        m_Grid->SetCellValue(row, c++, pcmn::Player::Position::ToString(params.m_Position));
        m_Grid->SetCellValue(row, c++, pcmn::BetSize::ToString(params.m_BetSize));
        m_Grid->SetCellValue(row, c++, pcmn::Player::Count::ToString(params.m_ActivePlayers));
        m_Grid->SetCellValue(row, c++, pcmn::Danger::ToString(params.m_Danger));
        m_Grid->SetCellValue(row, c++, pcmn::Player::Style::ToString(params.m_BotAverageStyle));
        m_Grid->SetCellValue(row, c++, pcmn::Player::Style::ToString(params.m_BotStyle));
        m_Grid->SetCellValue(row, c++, pcmn::StackSize::ToString(params.m_BotStackSize));
        
        if (params.m_CheckFold)
            m_Grid->SetCellValue(row, c++, "Check/Fold");
        else
        if (params.m_CheckCall)
            m_Grid->SetCellValue(row, c++, "Check/Call");
        else
        if (params.m_BetRaise)
            m_Grid->SetCellValue(row, c++, "Bet/Raise");
        else
            m_Grid->SetCellValue(row, c++, "Not set");

        m_Grid->SetRowLabelValue(row, boost::lexical_cast<std::string>(paramsRow));
    }
}

void Teacher::SetPramsValues()
{
    for (unsigned i = 0; i < m_Parameters.size(); ++i)
    {
        neuro::Params& params = m_Parameters[i];
        params.SetParams(i);
    }
}

void Teacher::OnGridScroll(wxMouseEvent& event)
{
    if (event.m_wheelRotation > 0)
    {
        // up
        GridUp();
    }
    else
    if (event.m_wheelRotation < 0)
    {
        // down 
        GridDown();
    }
}

void Teacher::SetRowViewToCurrentParams()
{
    const unsigned hash = m_CurrentParams.Hash();
    m_CurrentRow = hash;
    UpdateGrid(hash);
}

void Teacher::OnKeyDown(wxKeyEvent& event)
{
    if (event.m_rawCode == 38)
    {
        // up
        GridUp();
    }
    else
    if (event.m_rawCode == 40)
    {
        // down
        GridDown();
    }

    if (event.m_rawCode == 33)
    {
        // pg up
        GridUp(m_Grid->GetRows() * 5);
    }
    else
    if (event.m_rawCode == 34)
    {
        // pg down
        GridDown(m_Grid->GetRows() * 5);
    }

    if (event.m_uniChar != 'R' && event.m_uniChar != 'C' && event.m_uniChar != 'F')
        return;

    m_CurrentParams.m_CheckFold = false;
    m_CurrentParams.m_CheckCall = false;
    m_CurrentParams.m_BetRaise = false;

    switch (event.m_uniChar)
    {
    case 'F': m_CurrentParams.m_CheckFold = true; break;
    case 'C': m_CurrentParams.m_CheckCall = true; break;
    case 'R': m_CurrentParams.m_BetRaise = true; break;
    }

    const neuro::Params::List temp = boost::assign::list_of(m_CurrentParams);
    MergeParams(m_Parameters, temp);
    GridDown();
}

void Teacher::GridUp(unsigned amount)
{
    UpdateGrid(m_CurrentRow > amount ? m_CurrentRow -= amount : 0);
    m_CurrentParams = m_Parameters[m_CurrentRow];
    SetGuiParams(m_CurrentParams);
}

void Teacher::GridDown(unsigned amount)
{
    UpdateGrid((m_CurrentRow + amount) < m_Parameters.size() ? m_CurrentRow += amount : m_CurrentRow);
    m_CurrentParams = m_Parameters[m_CurrentRow];
    SetGuiParams(m_CurrentParams);
}

void Teacher::TrainCallback(unsigned epoch, unsigned epochCount, float error, float desiredError, const boost::posix_time::ptime& start)
{
    m_Gauge->SetValue((epoch * 100) / epochCount);

    boost::posix_time::ptime now = boost::posix_time::microsec_clock().local_time();
    const boost::posix_time::time_duration td(now - start);
    const __int64 milliseconds = td.total_milliseconds();

    const double oneEpochTime = milliseconds / epoch;

    const unsigned elapsedEpochs = epochCount - epoch;
    now += boost::posix_time::milliseconds(elapsedEpochs * oneEpochTime);

    std::ostringstream oss;
    oss 
        << "epoch: " << epoch 
        << ", total: " << epochCount 
        << ", error: " << error 
        << ", desired error: " << desiredError
        << ", elapsed minutes: " << milliseconds / (60 * 1000)
        << ", seconds: " << milliseconds / 1000
        << ", complete in: " << boost::posix_time::to_simple_string(now);

    m_StatusBar->SetStatusText(oss.str().c_str());
}

void Teacher::OnRead(wxCommandEvent& event)
{
    try
    {
        Log logger;
        sql::SQLiteDataBase db(logger);
        db.Open(cfg::NETWORK_FILE_NAME);

        const std::size_t count = neuro::Params::ReadAll(m_Parameters, db);

        std::ostringstream oss;
        oss << "read " << count << " params from " << cfg::NETWORK_FILE_NAME;
        m_StatusBar->SetStatusText(oss.str());
    }
    catch (const std::exception& e)
    {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

void Teacher::OnWrite(wxCommandEvent& event)
{
    Log logger;
    sql::SQLiteDataBase db(logger);

    try
    {
        db.Open(cfg::NETWORK_FILE_NAME);
        db.BeginTransaction();

        neuro::Params::WriteAll(m_Parameters, db);
        db.Commit();

        std::ostringstream oss;
        oss << "wrote " << m_Parameters.size() << " params to " << cfg::NETWORK_FILE_NAME;
        m_StatusBar->SetStatusText(oss.str());
    }
    catch (const std::exception& e)
    {
        db.Rollback();
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

void Teacher::OnExecute(wxCommandEvent& event)
{
    Log logger;
    sql::SQLiteDataBase db(logger);

    try
    {
        db.Open(cfg::NETWORK_FILE_NAME);
        db.BeginTransaction();

        std::string text = m_QueryText->GetValue().ToStdString();
        if (m_QueryText->HasSelection())
        {
            long begin = 0;
            long end = 0;
            m_QueryText->GetSelection(&begin, &end);

            text.assign(text.substr(begin, end - begin));
        }

        const std::size_t rows = db.Execute(text);
        db.Commit();

        neuro::Params::ReadAll(m_Parameters, db);

        std::ostringstream oss;
        oss << "affected " << rows << " rows ";
        m_StatusBar->SetStatusText(oss.str());
    }
    catch (const std::exception& e)
    {
        db.Rollback();
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

}