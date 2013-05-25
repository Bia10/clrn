#include "Settings.h"


namespace clrn{

const TCHAR * const CSettings::SETTINGS_FILE_NAME = _T("Settings.xml");

CSettings::CSettings(void) 
{
}

CSettings::~CSettings(void)
{
}

void CSettings::LoadDefault()
{
	m_nACPFrequency				= 1840000;
	m_nCAPFrequency				= 1370000;
	m_nMinimumPeriodSize		= 1000;
	m_nPeriodsPerBuffer			= 10;
	m_nIterationCoefficient		= 10;
	m_nMinIterations			= 10;
	m_nMaxIterations			= 1000;
	m_nFindZeroCoefficient		= 10000;
	m_nApproximatePoints		= 5;
	m_nRShunt					= 1000;
	m_sCalibrationFileName		= _T("calibration.xml");
	m_sMeasurementFileName		= _T("measurement.xml");
	m_sAmplitudesFileName		= _T("amplitudes.xml");
	m_sCoefficientsFileName		= _T("coefficients.txt");
	m_sLoggerFileName			= _T("CypressGUILog.txt");
	m_eLogLevel					= CLog::Level::eAll;
}

void CSettings::Save()
{
	TRY_EXCEPTIONS
	{
		CXmlNode xmlSettings;
		xmlSettings.Data[TAG_NAME] = _T("Settings");

		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("ACPFrequency");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nACPFrequency);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("CAPFrequency");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nCAPFrequency);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("MinimumPeriodSize");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nMinimumPeriodSize);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("PeriodsPerBuffer");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nPeriodsPerBuffer);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("IterationCoefficient");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nIterationCoefficient);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("MinIterations");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nMinIterations);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("MaxIterations");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nMaxIterations);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("FindZeroCoefficient");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nFindZeroCoefficient);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("ApproximatePoints");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nApproximatePoints);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("RShunt");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (m_nRShunt);

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("CalibrationFileName");
			xmlCurrent.Data[TAG_VALUE]	= m_sCalibrationFileName;

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("MeasurementFileName");
			xmlCurrent.Data[TAG_VALUE]	= m_sMeasurementFileName;

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("AmplitudesFileName");
			xmlCurrent.Data[TAG_VALUE]	= m_sAmplitudesFileName;

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("CoefficientsFileName");
			xmlCurrent.Data[TAG_VALUE]	= m_sCoefficientsFileName;

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("LoggerFileName");
			xmlCurrent.Data[TAG_VALUE]	= m_sLoggerFileName;

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		{
			CXmlNode xmlCurrent;
			xmlCurrent.Data[TAG_NAME]	= _T("LogLevel");
			xmlCurrent.Data[TAG_VALUE]	= boost::lexical_cast< _tstring > (static_cast< int > (m_eLogLevel));

			xmlSettings.Childs.push_back(xmlCurrent);
		}
		CXmlParser Parser;
		Parser.SetData(xmlSettings);
		Parser.SaveToFile(SETTINGS_FILE_NAME);
	}
	CATCH_PASS_EXCEPTIONS;
}


void CSettings::Load()
{
	TRY_EXCEPTIONS
	{
		CXmlParser Parser;
		Parser.ParseFile(SETTINGS_FILE_NAME);
		CXmlNode xmlSettings = Parser.GetData();

		{
			const _tstring sWhat = _T("ACPFrequency");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nACPFrequency = boost::lexical_cast< long > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("CAPFrequency");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nCAPFrequency = boost::lexical_cast< long > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("MinimumPeriodSize");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nMinimumPeriodSize = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("PeriodsPerBuffer");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nPeriodsPerBuffer = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("IterationCoefficient");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nIterationCoefficient = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("MinIterations");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nMinIterations = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("MaxIterations");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nMaxIterations = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("FindZeroCoefficient");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nFindZeroCoefficient = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("ApproximatePoints");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nApproximatePoints = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("RShunt");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_nRShunt = boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]);
		}
		{
			const _tstring sWhat = _T("CalibrationFileName");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_sCalibrationFileName = pCurrent->Data[TAG_VALUE];
		}
		{
			const _tstring sWhat = _T("MeasurementFileName");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_sMeasurementFileName = pCurrent->Data[TAG_VALUE];
		}
		{
			const _tstring sWhat = _T("AmplitudesFileName");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_sAmplitudesFileName = pCurrent->Data[TAG_VALUE];
		}
		{
			const _tstring sWhat = _T("CoefficientsFileName");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_sCoefficientsFileName = pCurrent->Data[TAG_VALUE];
		}
		{
			const _tstring sWhat = _T("LoggerFileName");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_sLoggerFileName = pCurrent->Data[TAG_VALUE];
		}
		{
			const _tstring sWhat = _T("LogLevel");
			CXmlNode * pCurrent = xmlSettings.FindNode(TAG_NAME, sWhat);
			if (pCurrent)
				m_eLogLevel = static_cast< CLog::Level::Enum_t > (boost::lexical_cast< int > (pCurrent->Data[TAG_VALUE]));
		}
	}
	CATCH_PASS_EXCEPTIONS;
}

}

clrn::CSettings Settings::s_Settings;

clrn::CSettings& Settings::Instance() 
{
	return s_Settings;
}
