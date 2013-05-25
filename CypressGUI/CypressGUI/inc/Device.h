#pragma once

//! Include project headers
#include "stdafx.h"
#include "File.h"
#include "DataAnalyzer.h"
#include "XmlParser.h"
#include "Log.h"
#include "RangeDlg.h"
#include "Exception.h"
#include "Settings.h"

//! Include Stl library headers
#include <map>

//! Include Boost library headers
#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_array.hpp>
#include <boost/function.hpp>

namespace clrn{

//! ���������� USB ���������
class CDevice
{
public:
	CDevice(CLog& Log);
	~CDevice(void);

	//! ��� ������� �������� ���������� ��������
	typedef boost::function< void (void) > TCallBack;

	//! ����� ������ ����������
	struct Mode
	{
		enum Enum_t 
		{
			eSingleFrequency,
			eCalibration,
			eMeasurement,
			eReadOnly,
			eWriteOnly
		};
	};

	//! ��� ������������ ����
	struct Circuit
	{
		enum Enum_t
		{
			eNone,
			eCircuit
		};
	};

	//! ���� ���������� ������
	struct Data
	{
		enum Enum_t
		{
			eOutgoingSinus,
			eIncomingSinus,
			ePhase,
			eAmplitude,
			eFrequency,
			eMeasurementData,
			eOutgoingPhase,
			eIncomingPhase,
		};
	};

	//! ��������� ����������
	void										Start(const Mode::Enum_t eMode, 
													const std::vector< CRange >& Ranges,
													TCallBack& CallBack = TCallBack()
													);

	//! ���������� ����������
	void										Stop();

	//! �������� �������
	void										ClearData();

	//! ���������� ������������ ����
	void										SetConnectedCircuit(const Circuit::Enum_t eCircuit);

	//! ���������������� ������ ��� ��������� ������
	void										RegisterOutputData(void * pData, Data::Enum_t eType, boost::mutex& mxOutData);

	//! �������� �������� ������
	const unsigned __int64						GetReadSpeed() const;

	//! �������� �������� ������
	const unsigned __int64						GetWriteSpeed() const;

	//! �������� ������� ����
	const float									GetPhase() const;

	//! �������� ������� XML ������
	const CXmlNode								GetData() const;

	//! �������� ���-�� ������������ ����-�
	const int									GetDeviceCount() const;

private:

	//! ���������� ����������
	const int									Connect();

	//! ��������� ����������
	void										Disconnect();

	//! ��������� �-��� ��������� �� ����� �������
	void										ThreadSingle();

	//! ��������� �-��� ��������� ��������� ������
	void										ThreadMultiple(const std::vector< CRange >& Ranges,
																TCallBack& CallBack = TCallBack()
																);
	//! ��������� �-��� ������
	void										ThreadRead();

	//! ��������� �-��� ������
	void										ThreadWrite();

	//! �������� ����������� ����
	void										SendControlBytes(const int nValue);

	//! ��������� �����������
	bool										IncreaseInAmplitude();

	//! ��������� �����������
	bool										DecreaseInAmplitude();

	//! ���������� ���������� ������
	bool										ProcessInputData();

	//! ���������������� �������� ���������
	bool										CorrectInAmplitude(std::vector< int >& Data);

	//! ���������������� ��������� ���������
	bool										CorrectOutAmplitude(const float nEtalonAmplitude);

	//! ��������� ������ � ����
	void										SaveData(const _tstring& sPath = _T(""));

	//! ���������� ����� ������
	void										StopWriteThread();

	//! ���������� ����� ������
	void										StopReadThread();

	//! ��������� ����� ������
	void										StartWriteThread();

	//! ��������� ����� ������
	void										StartReadThread();

	//! �������� ������ � ���������, ����������� ����� RegisterOutputData()
	void										SendOutData();

	//! ���������� ������ ������
	void										CalcBufferSize();

	//! ��������� ������������� ����������
	void										LoadAmplitudesData();

	//! �������� �������� ��������� ���������
	const float									GetOutAmplitude();

	//! ������ �� ������
	CLog&										m_Log;

	//! ������ API ����������� �� ������
	boost::scoped_ptr< CCyUSBDevice >			m_pUsbDeviceRead;

	//! ������ API ����������� �� ������
	boost::scoped_ptr< CCyUSBDevice >			m_pUsbDeviceWrite;

	//! ������ ��� ��������� ������
	CAnalyzer									m_Analyzer;

	//! ������� �������
	long										m_nFrequency;

	//! ������� ����
	float										m_fPhase;

	//! ������� ���������
	int											m_nAmplitude;

	//! ������� �������� �����
	std::list < int >							m_InSinus;

	//! ������� ��������� ����
	std::list < int >							m_OutPhase;

	//! ������� �������� ����
	std::list < int >							m_InPhase;

	//! ������� ��������� �����
	std::list < int >							m_OutSinus;

	//! ���� ������ ��������� ������
	bool										m_bIsWorking;

	//! ������� �����������
	float										m_nCoefficient;

	//! ������� ��������� ������������� ������
	float										m_nOutSinusAmplitude;

	//! ������������
	std::map< float, float >					m_Coefficients;

	//! ��������� �� �������� ������
	std::map< Data::Enum_t, void * >			m_OutData;

	//! ��������� �� ������� ���������� �������� ������
	boost::mutex *								m_pmxOutData;

	//! ������� XML ������
	CXmlNode									m_XmlData;

	//! XML ������ ��������� ��������
	CXmlNode									m_AmplitudeXmlData;

	//! ������������ ����
	Circuit::Enum_t								m_eCircuit;

	//! ������� ������
	std::list< UCHAR * >						m_InData;

	//! ������� �� ������� ������
	boost::mutex								m_mxInData;

	//! ����� ������
	boost::thread								m_threadRead;

	//! ����� ������
	boost::thread								m_threadWrite;

	//! ������� ����� ���������
	boost::thread								m_threadWork;

	//! �������� ������
	unsigned __int64							m_nReadSpeed;

	//! �������� ������
	unsigned __int64							m_nWriteSpeed;

	//! ����� ��������� ������������� ���������
	unsigned __int64							m_nLastCorrectTime;

	//! ������ ������
	int											m_nBufferSize;

};

}