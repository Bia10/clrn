#pragma once

//! Include project headers
#include "stdafx.h"
#include "XmlParser.h"
#include "Log.h"
#include "Exception.h"

//! Include Stl library headers
#include <string>

//! Include Boost library headers
#include <boost/thread/recursive_mutex.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

namespace clrn{

//! Реализация настроек приложения
class CSettings : 
	private boost::recursive_mutex,
	private boost::noncopyable
{
public:

	//! Конструктор
	CSettings();

	//! Деструктор
	~CSettings(void);

	//! Сохранить настройки в файл
	void								Save();

	//! Загрузить настройки из файла
	void								Load();

	//! Установить по умолчанию
	void								LoadDefault();

	//!@{ Методы получения настроек
	inline const unsigned long			GetACPFrequency()											{boost::recursive_mutex::scoped_lock Lock(*this); return m_nACPFrequency;				};
	inline const unsigned long			GetCAPFrequency()											{boost::recursive_mutex::scoped_lock Lock(*this); return m_nCAPFrequency;				};

	inline const unsigned int			GetMinimumPeriodSize()										{boost::recursive_mutex::scoped_lock Lock(*this); return m_nMinimumPeriodSize;			};
	inline const unsigned int			GetPeriodsPerBuffer()										{boost::recursive_mutex::scoped_lock Lock(*this); return m_nPeriodsPerBuffer;			};
	inline const unsigned int			GetIterationCoefficient()									{boost::recursive_mutex::scoped_lock Lock(*this); return m_nIterationCoefficient;		};
	inline const unsigned int			GetMinIterations()											{boost::recursive_mutex::scoped_lock Lock(*this); return m_nMinIterations;				};
	inline const unsigned int			GetMaxIterations()											{boost::recursive_mutex::scoped_lock Lock(*this); return m_nMaxIterations;				};
	inline const unsigned int			GetFindZeroCoefficient()									{boost::recursive_mutex::scoped_lock Lock(*this); return m_nFindZeroCoefficient;		};
	inline const unsigned int			GetApproximatePoints()										{boost::recursive_mutex::scoped_lock Lock(*this); return m_nApproximatePoints;			};
	inline const unsigned int			GetRShunt()													{boost::recursive_mutex::scoped_lock Lock(*this); return m_nRShunt;						};

	inline const _tstring				GetCalibrationFileName()									{boost::recursive_mutex::scoped_lock Lock(*this); return m_sCalibrationFileName;		};
	inline const _tstring				GetMeasurementFileName()									{boost::recursive_mutex::scoped_lock Lock(*this); return m_sMeasurementFileName;		};
	inline const _tstring				GetAmplitudesFileName()										{boost::recursive_mutex::scoped_lock Lock(*this); return m_sAmplitudesFileName;			};
	inline const _tstring				GetCoefficientsFileName()									{boost::recursive_mutex::scoped_lock Lock(*this); return m_sCoefficientsFileName;		};
	inline const _tstring				GetLoggerFileName()											{boost::recursive_mutex::scoped_lock Lock(*this); return m_sLoggerFileName;				};
	inline const CLog::Level::Enum_t	GetLogLevel()												{boost::recursive_mutex::scoped_lock Lock(*this); return m_eLogLevel;					};

	//!}

	//!@{ Методы установки настроек
	inline void							SetACPFrequency(const unsigned long &nValue)				{boost::recursive_mutex::scoped_lock Lock(*this); m_nACPFrequency = nValue;				};
	inline void							SetCAPFrequency(const unsigned long &nValue)				{boost::recursive_mutex::scoped_lock Lock(*this); m_nCAPFrequency = nValue;				};
	
	inline void							SetMinimumPeriodSize(const unsigned int &nValue)			{boost::recursive_mutex::scoped_lock Lock(*this); m_nMinimumPeriodSize = nValue;		};
	inline void							SetPeriodsPerBuffer(const unsigned int &nValue)				{boost::recursive_mutex::scoped_lock Lock(*this); m_nPeriodsPerBuffer = nValue;			};
	inline void							SetIterationCoefficient(const unsigned int &nValue)			{boost::recursive_mutex::scoped_lock Lock(*this); m_nIterationCoefficient = nValue;		};
	inline void							SetMinIterations(const unsigned int &nValue)				{boost::recursive_mutex::scoped_lock Lock(*this); m_nMinIterations = nValue;			};
	inline void							SetMaxIterations(const unsigned int &nValue)				{boost::recursive_mutex::scoped_lock Lock(*this); m_nMaxIterations = nValue;			};
	inline void							SetFindZeroCoefficient(const unsigned int &nValue)			{boost::recursive_mutex::scoped_lock Lock(*this); m_nFindZeroCoefficient = nValue;		};
	inline void							SetApproximatePoints(const unsigned int &nValue)			{boost::recursive_mutex::scoped_lock Lock(*this); m_nApproximatePoints = nValue;		};
	inline void							SetRShunt(const unsigned int &nValue)						{boost::recursive_mutex::scoped_lock Lock(*this); m_nRShunt = nValue;					};

	inline void							SetCalibrationFileName(const _tstring& sName)				{boost::recursive_mutex::scoped_lock Lock(*this); m_sCalibrationFileName = sName;		};
	inline void							SetMeasurementFileName(const _tstring& sName)				{boost::recursive_mutex::scoped_lock Lock(*this); m_sMeasurementFileName = sName;		};
	inline void							SetAmplitudesFileName(const _tstring& sName)				{boost::recursive_mutex::scoped_lock Lock(*this); m_sAmplitudesFileName = sName;		};
	inline void							SetCoefficientsFileName(const _tstring& sName)				{boost::recursive_mutex::scoped_lock Lock(*this); m_sCoefficientsFileName = sName;		};
	inline void							SetLoggerFileName(const _tstring& sName)					{boost::recursive_mutex::scoped_lock Lock(*this); m_sLoggerFileName = sName;			};
	inline void							SetLogLevel(const CLog::Level::Enum_t eLevel)				{boost::recursive_mutex::scoped_lock Lock(*this); m_eLogLevel = eLevel;					};
	//!@}

private:
	
	//! Частота АЦП
	unsigned long						m_nACPFrequency;

	//! Частота ЦАП
	unsigned long						m_nCAPFrequency;

	//! Минимальный объем одного периода колебания
	unsigned int						m_nMinimumPeriodSize;

	//! Кол-во периодов на один буффер данных
	unsigned int						m_nPeriodsPerBuffer;

	//! Коэффициент для вычисления итераций
	unsigned int						m_nIterationCoefficient;

	//! Минимальное число итераций
	unsigned int						m_nMinIterations;

	//! Максимальное число итераций
	unsigned int						m_nMaxIterations;

	//! Коэффициент для определения "0" синуса
	unsigned int						m_nFindZeroCoefficient;

	//! Кол-во точек для аппроксимации
	unsigned int						m_nApproximatePoints;

	//! Шунтирующее сопротивление
	unsigned int						m_nRShunt;

	//! Имя временного файла калибровочных данных
	_tstring							m_sCalibrationFileName;

	//! Имя временного файла данных измерений
	_tstring							m_sMeasurementFileName;

	//! Имя файла с данными исходящих амплитуд
	_tstring							m_sAmplitudesFileName;

	//! Имя файла с коэффициентами усиления
	_tstring							m_sCoefficientsFileName;

	//! Имя файла логгера
	_tstring							m_sLoggerFileName;

	//! Уровень логирования
	CLog::Level::Enum_t					m_eLogLevel;

	//! Имя файла настроек
	static const TCHAR * const SETTINGS_FILE_NAME;
};


}

//! Синглтон для класса CSettings
class Settings
{
public:

	//! Ссылка на единственный объект настроек
	static clrn::CSettings& Instance();

private:

	//! Единственный объект настроек
	static clrn::CSettings s_Settings;
};