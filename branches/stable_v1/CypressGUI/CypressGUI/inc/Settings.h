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

//! ���������� �������� ����������
class CSettings : 
	private boost::recursive_mutex,
	private boost::noncopyable
{
public:

	//! �����������
	CSettings();

	//! ����������
	~CSettings(void);

	//! ��������� ��������� � ����
	void								Save();

	//! ��������� ��������� �� �����
	void								Load();

	//! ���������� �� ���������
	void								LoadDefault();

	//!@{ ������ ��������� ��������
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

	//!@{ ������ ��������� ��������
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
	
	//! ������� ���
	unsigned long						m_nACPFrequency;

	//! ������� ���
	unsigned long						m_nCAPFrequency;

	//! ����������� ����� ������ ������� ���������
	unsigned int						m_nMinimumPeriodSize;

	//! ���-�� �������� �� ���� ������ ������
	unsigned int						m_nPeriodsPerBuffer;

	//! ����������� ��� ���������� ��������
	unsigned int						m_nIterationCoefficient;

	//! ����������� ����� ��������
	unsigned int						m_nMinIterations;

	//! ������������ ����� ��������
	unsigned int						m_nMaxIterations;

	//! ����������� ��� ����������� "0" ������
	unsigned int						m_nFindZeroCoefficient;

	//! ���-�� ����� ��� �������������
	unsigned int						m_nApproximatePoints;

	//! ����������� �������������
	unsigned int						m_nRShunt;

	//! ��� ���������� ����� ������������� ������
	_tstring							m_sCalibrationFileName;

	//! ��� ���������� ����� ������ ���������
	_tstring							m_sMeasurementFileName;

	//! ��� ����� � ������� ��������� ��������
	_tstring							m_sAmplitudesFileName;

	//! ��� ����� � �������������� ��������
	_tstring							m_sCoefficientsFileName;

	//! ��� ����� �������
	_tstring							m_sLoggerFileName;

	//! ������� �����������
	CLog::Level::Enum_t					m_eLogLevel;

	//! ��� ����� ��������
	static const TCHAR * const SETTINGS_FILE_NAME;
};


}

//! �������� ��� ������ CSettings
class Settings
{
public:

	//! ������ �� ������������ ������ ��������
	static clrn::CSettings& Instance();

private:

	//! ������������ ������ ��������
	static clrn::CSettings s_Settings;
};