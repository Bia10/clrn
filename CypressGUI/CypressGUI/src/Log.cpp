#include "Log.h"

std::map< DWORD, int > CLog::CScopedLog::s_Nesting;
const TCHAR * const CLog::MESSAGE_TEMPLATE = _T("[%s:%s] [%s] %s %s");

CLog::CLog(void) :
	m_eLevel(Level::eOff),
	m_pListBox(0)
{
}

CLog::~CLog(void)
{
}

void CLog::Open(const _tstring& sPath, const Level::Enum_t eLevel, const int nSizeX, const int nSizeY)
{
	boost::recursive_mutex::scoped_lock Lock(*this);

	if (!m_pLoggerWindow)
		m_pLoggerWindow.reset(new CLogWnd(CLogWnd::TResizeFun(boost::bind(&CLog::OnResize, this))));

	if (!m_pListBox)
		m_pListBox.reset(new CListBox());

	RECT WindowRect;
	m_pLoggerWindow->GetWindowRect(&WindowRect);

	RECT ListBoxRect;
	ListBoxRect.left = 0;
	ListBoxRect.top = 0;
	ListBoxRect.bottom = WindowRect.bottom - WindowRect.top - 30;
	ListBoxRect.right = WindowRect.right - WindowRect.left - 10;

	m_pListBox->Create(
		WS_CHILD | WS_VISIBLE| LBS_STANDARD | WS_HSCROLL, 
		ListBoxRect, 
		m_pLoggerWindow.get(), 
		1);

	OnResize();
	HideWindow();

	m_sPath = sPath;
	m_eLevel = eLevel;
	m_Ofs.open(m_sPath.c_str(), std::ios_base::out);
	m_Ofs.close();
	m_Ofs.open(m_sPath.c_str(), std::ios_base::app);
	Write(_T("Logger started."));
}

void CLog::OnResize()
{
	if (!m_pLoggerWindow)
		return;

	RECT WindowRect;
	m_pLoggerWindow->GetWindowRect(&WindowRect);

	RECT ListBoxRect;
	ListBoxRect.left = 0;
	ListBoxRect.top = 0;
	ListBoxRect.bottom = WindowRect.bottom - WindowRect.top - 30;
	ListBoxRect.right = WindowRect.right - WindowRect.left - 10;

	m_pListBox->SetWindowPos(NULL, 0, 0, ListBoxRect.right, ListBoxRect.bottom, SWP_NOMOVE | SWP_NOZORDER);
}

void CLog::Clear()
{
	boost::recursive_mutex::scoped_lock Lock(*this);

	if (m_pListBox)
		m_pListBox->ResetContent();

	Shutdown();
	m_Ofs.open(m_sPath.c_str(), std::ios_base::app);
	Write(_T("Logger started."));
}

void CLog::ShowWindow()
{
	m_pLoggerWindow->ShowWindow(SW_SHOWNORMAL);
	m_pLoggerWindow->UpdateWindow();
}


void CLog::HideWindow()
{
	m_pLoggerWindow->ShowWindow(SW_HIDE);
	m_pLoggerWindow->UpdateWindow();
}


void CLog::Shutdown()
{
	boost::recursive_mutex::scoped_lock Lock(*this);
	m_Ofs.close();
}

void CLog::SetLevel(const Level::Enum_t eLevel)
{
	m_eLevel = eLevel;
}

const _tstring CLog::GetText(const _tstring& sMessage)
{
	CTime Current;
	const _tstring sTime(Current.GetCurrentTime().Format("%H:%M:%S").GetBuffer());
	__int64 nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
	return (_tformat(MESSAGE_TEMPLATE) 
		% sTime % nTime % GetCurrentThreadId() % _T(" ### ") % sMessage).str();
}

void CLog::Write(const std::string& sMessage)
{
	if (Level::eOff == m_eLevel || !m_Ofs.is_open())
		return;

#ifdef _UNICODE
	const std::wstring sWiden = vr::str::widen(sMessage, std::locale());
	const _tstring sText = GetText(sWiden);
	m_Ofs << sText << std::endl;
	Write2ListBox(sWiden);
#else
	const _tstring sText = GetText(sMessage);
	m_Ofs << sText << std::endl;
	Write2ListBox(sMessage);
#endif
}

void CLog::Write(const std::wstring& sMessage)
{
	if (Level::eOff == m_eLevel || !m_Ofs.is_open())
		return;

#ifdef _UNICODE
	const _tstring sText = GetText(sMessage);
	m_Ofs << sText << std::endl;
	Write2ListBox(sMessage);
#else
	const std::string sNarrow = vr::str::narrow(sMessage, std::locale());
	const _tstring sText = GetText(sNarrow);
	m_Ofs << sText << std::endl;
	Write2ListBox(sNarrow);
#endif
}


void CLog::WriteScoped(const _tstring& sText)
{
	TRY_EXCEPTIONS
	{
	if (Level::eOff == m_eLevel || Level::eMessagesOnly == m_eLevel)
		return;

		m_Ofs << sText << std::endl;
		if (Level::eAllVerbose == m_eLevel && m_pListBox)
		{
			m_pListBox->AddString(sText.c_str());
			int nCount = m_pListBox->GetCount();
			m_pListBox->SetCurSel(nCount - 1);
		}
	}
	CATCH_IGNORE_EXCEPTIONS
}

void CLog::Write2ListBox(const _tstring& sMessage)
{
	if (!m_pListBox)
		return;
	TRY_EXCEPTIONS
	{
		CTime Current;
		_tstring sText(Current.GetCurrentTime().Format(_T("[%H:%M:%S] ")).GetBuffer());
		sText += sMessage;
		_tstring::size_type nLast = 0;
		_tstring::size_type nCL = sText.find('\n');
		while (nCL != _tstring::npos)
		{
			const _tstring sTemp(&sText[nLast], &sText[nCL]);
			int nCount = m_pListBox->GetCount();
			m_pListBox->InsertString(nCount, sTemp.c_str());
			nLast = nCL + 1;
			nCL = sText.find('\n', nLast);
		}
		int nCount = m_pListBox->GetCount();
		m_pListBox->InsertString(nCount, &sText[nLast]);
		m_pListBox->SetCurSel(nCount);
	}
	CATCH_IGNORE_EXCEPTIONS
}

void CLog::Write(const _tformat& Message)
{
	boost::recursive_mutex::scoped_lock Lock(*this);
	Write(Message.str());
}

void CLog::WriteProgressMessage(const _tformat& Message, const int nIteration)
{
	if (Level::eOff == m_eLevel || !m_Ofs.is_open())
		return;

		_tstring sText = GetText(Message.str());
		m_Ofs << sText << std::endl;

		if (!m_pListBox)
			return;	

	TRY_EXCEPTIONS
	{
		CTime Current;
		sText.assign(Current.GetCurrentTime().Format(_T("[%H:%M:%S] ")).GetBuffer());
		sText += Message.str();
		const int nCount = m_pListBox->GetCount();
		if (!nIteration)
			m_pListBox->InsertString(nCount, sText.c_str());
		else
		{
			m_pListBox->InsertString(nCount - 1, sText.c_str());
			m_pListBox->DeleteString(nCount);
		}
		m_pListBox->SetCurSel(nCount - 1);
	}
	CATCH_IGNORE_EXCEPTIONS
}