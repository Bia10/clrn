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

	//! Уровни логирования
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

	//! Создать лог
	void Open(const _tstring& sPath, const Level::Enum_t eLevel, const int nSizeX = 200, const int nSizeY = 200);

	//! Закрыть тог
	void Shutdown();

	//! Очистить лог
	void Clear();

	//! Обработать изменение размера окна логгера
	void OnResize();

	//! Установить уровень логирования
	void SetLevel(const Level::Enum_t eLevel);

	//! Отобразить окно логгера
	void ShowWindow();

	//! Скрыть окно логгера
	void HideWindow();

	//! Записать
	void Write(const std::string& sMessage);

	//! Записать
	void Write(const std::wstring& sMessage);

	//! Записать
	void Write(const _tformat& Message);

	//! Записать сообщение, отображающее прогресс
	void WriteProgressMessage(const _tformat& Message, const int nIteration = 0);

	//! Класс для логирования входа/выхода из ф-ций
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

		//! Получить текст строки лога
		const _tstring GetText(bool bIn)
		{		
			CTime Current;
			const _tstring sTime(Current.GetCurrentTime().Format(_T("%H:%M:%S")).GetBuffer());
			__int64 nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
			return (_tformat(MESSAGE_TEMPLATE) 
				% sTime % nTime % m_dwThreadId % GetNesting(bIn) % m_sFunction).str();
				
			return _T("");
		}

		//! Получить вложенность для текущего потока
		const _tstring GetNesting(const bool bIn)
		{
			_tstring sTemp;
			sTemp.resize(s_Nesting[m_dwThreadId], '-');
			if (bIn)
				return sTemp + _T("->");
			return _tstring(_T("<-")) + sTemp;
		}

		//! Ссылка на логгер
		CLog&								m_Log;

		//! Имя функции
		const _tstring						m_sFunction;

		//! Карта смещений и потоков
		static std::map< DWORD, int >		s_Nesting;

		//! ID потока, использующего логгер
		DWORD								m_dwThreadId;
	};
	

private:

	//! Записать сообщение CScopedLog
	void									WriteScoped(const _tstring& sText);

	//! Получить текст сообщения
	const _tstring							GetText(const _tstring& sMessage);

	//! Записать в листбокс
	void									Write2ListBox(const _tstring& sMessage);

	//! Файловый поток для записи
	_tofstream								m_Ofs;

	//! Путь к лог-файлу
	_tstring								m_sPath;	

	//! Уровень логирования
	Level::Enum_t							m_eLevel;

	//! Указатель на окно логгера
	boost::scoped_ptr< CLogWnd >			m_pLoggerWindow;

	//! Указатель на листбокс, для отображения пользователю
	boost::scoped_ptr< CListBox >			m_pListBox;

	//! Формат сообщений
	static const TCHAR * const				MESSAGE_TEMPLATE;
};


//! Макросы для удобства логирования
#define SCOPED_LOG(Log) CLog::CScopedLog __ScopedLog(Log, BOOST_CURRENT_FUNCTION);