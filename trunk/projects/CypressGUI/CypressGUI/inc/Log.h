#pragma once

//! Include project headers
#include "stdafx.h"
#include "File.h"
#include "Conversion.h"
#include "LogWnd.h"
#include "Exception.h"

//! Include Stl library headers
#include <fstream>
#include <string>
#include <map>
#include <vector>

//! Include Boost library headers
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>

class CLog : private boost::recursive_mutex
{
public:
	CLog(void);
	~CLog(void);

	//! ������ �����������
	struct Level
	{
		enum Enum_t
		{
			eOff,
			eMessagesOnly,
			eAll,
			eAllVerbose
		};
	};

	//! ������� ���
	void Open(const _tstring& sPath, const Level::Enum_t eLevel, const int nSizeX = 200, const int nSizeY = 200);

	//! ������� ���
	void Shutdown();

	//! �������� ���
	void Clear();

	//! ���������� ��������� ������� ���� �������
	void OnResize();

	//! ���������� ������� �����������
	void SetLevel(const Level::Enum_t eLevel);

	//! ���������� ���� �������
	void ShowWindow();

	//! ������ ���� �������
	void HideWindow();

	//! ��������
	void Write(const std::string& sMessage);

	//! ��������
	void Write(const std::wstring& sMessage);

	//! ��������
	void Write(const _tformat& Message);

	//! �������� ���������, ������������ ��������
	void WriteProgressMessage(const _tformat& Message, const int nIteration = 0);

	//! ����� ��� ����������� �����/������ �� �-���
	class CScopedLog
	{
	public:
		CScopedLog(CLog& Logger, const char * szCurrentFunction) : 
			m_Log(Logger),
#ifdef _UNICODE
			m_sFunction(vr::str::widen(szCurrentFunction, std::locale()))
#else
			m_sFunction(szCurrentFunction)
#endif
		{
			m_dwThreadId = GetCurrentThreadId();
			++s_Nesting[m_dwThreadId];
			m_Log.WriteScoped(GetText(true));
		}
		~CScopedLog()
		{
			m_Log.WriteScoped(GetText(false));
			--s_Nesting[m_dwThreadId];
		}
	private:

		//! �������� ����� ������ ����
		const _tstring GetText(bool bIn)
		{		
			CTime Current;
			const _tstring sTime(Current.GetCurrentTime().Format(_T("%H:%M:%S")).GetBuffer());
			__int64 nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
			return (_tformat(MESSAGE_TEMPLATE) 
				% sTime % nTime % m_dwThreadId % GetNesting(bIn) % m_sFunction).str();
				
			return _T("");
		}

		//! �������� ����������� ��� �������� ������
		const _tstring GetNesting(const bool bIn)
		{
			_tstring sTemp;
			sTemp.resize(s_Nesting[m_dwThreadId], '-');
			if (bIn)
				return sTemp + _T("->");
			return _tstring(_T("<-")) + sTemp;
		}

		//! ������ �� ������
		CLog&								m_Log;

		//! ��� �������
		const _tstring						m_sFunction;

		//! ����� �������� � �������
		static std::map< DWORD, int >		s_Nesting;

		//! ID ������, ������������� ������
		DWORD								m_dwThreadId;
	};
	

private:

	//! �������� ��������� CScopedLog
	void									WriteScoped(const _tstring& sText);

	//! �������� ����� ���������
	const _tstring							GetText(const _tstring& sMessage);

	//! �������� � ��������
	void									Write2ListBox(const _tstring& sMessage);

	//! �������� ����� ��� ������
	_tofstream								m_Ofs;

	//! ���� � ���-�����
	_tstring								m_sPath;	

	//! ������� �����������
	Level::Enum_t							m_eLevel;

	//! ��������� �� ���� �������
	boost::scoped_ptr< CLogWnd >			m_pLoggerWindow;

	//! ��������� �� ��������, ��� ����������� ������������
	boost::scoped_ptr< CListBox >			m_pListBox;

	//! ������ ���������
	static const TCHAR * const				MESSAGE_TEMPLATE;
};


//! ������� ��� �������� �����������
#define SCOPED_LOG(Log) CLog::CScopedLog __ScopedLog(Log, BOOST_CURRENT_FUNCTION);