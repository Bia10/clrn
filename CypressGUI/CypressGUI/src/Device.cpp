#include "Device.h"

namespace clrn{

CDevice::CDevice(CLog& Log) :
	m_Log(Log),
	m_Analyzer(CAnalyzer(Settings::Instance().GetACPFrequency(), Settings::Instance().GetCAPFrequency())),
	m_nOutSinusAmplitude(36),
	m_nBufferSize(10240),
	m_bIsWorking(false)
{
	SCOPED_LOG(m_Log);
}

CDevice::~CDevice(void)
{
	SCOPED_LOG(m_Log);
	Disconnect();
}
const unsigned __int64 CDevice::GetReadSpeed() const
{
	return m_nReadSpeed;
}


const unsigned __int64 CDevice::GetWriteSpeed() const
{
	return m_nWriteSpeed;
}

const float CDevice::GetPhase() const
{
	return m_fPhase;
}

const CXmlNode CDevice::GetData() const
{
	return m_XmlData;
}

void CDevice::SendControlBytes(const int nValue)
{
	SCOPED_LOG(m_Log);
	if (!m_pUsbDeviceRead)
		return;
	// Sending control bytes
	CCyControlEndPoint *ept = m_pUsbDeviceRead->ControlEndPt;

	const int nControlValue = (Circuit::eNone == m_eCircuit ? 0 : 0x40);
	int nValueToSend = nValue | nControlValue;

	if (ept)
	{
		ept->Target = TGT_DEVICE;
		ept->ReqType = REQ_VENDOR;
		ept->Direction = DIR_TO_DEVICE;
		ept->ReqCode = 0xB2;
		ept->Value = 256;
		ept->Index = 0;
		UCHAR buf[2];
		ZeroMemory(buf, 2);
		buf[0] = nValueToSend;
		buf[1] = nValueToSend;

		LONG buflen = 2;
		ept->XferData(buf, buflen);
		ept->XferData(buf, buflen);
	}
	else
	{
		m_Log.Write(_T("Failed to send control bytes."));
	}
}

bool CDevice::IncreaseInAmplitude()
{
	SCOPED_LOG(m_Log);

	//! Итератор с текущим значением коеффициента
	std::map< float, float >::iterator it = m_Coefficients.find(m_nCoefficient);
	++it;
	if (it != m_Coefficients.end())
	{
		SendControlBytes(it->second);
		SendControlBytes(it->second);
		SendControlBytes(it->second);
		m_Log.Write(_tformat(_T("Changing Coefficient from [%s] to [%s]")) % m_nCoefficient % it->first);
		m_nCoefficient = it->first;
		return true;
	}
	m_Log.Write(_T("Coefficient is maximal!"));
	return false;
}


bool CDevice::DecreaseInAmplitude()
{
	SCOPED_LOG(m_Log);
	//! Итератор с текущим значением коеффициента
	std::map< float, float >::iterator it = m_Coefficients.find(m_nCoefficient);
	if (it != m_Coefficients.begin())
	{
		--it;
		SendControlBytes(it->second);
		SendControlBytes(it->second);
		SendControlBytes(it->second);
		m_Log.Write(_tformat(_T("Changing Coefficient from [%s] to [%s]")) % m_nCoefficient % it->first);
		m_nCoefficient = it->first;
		return true;
	}
	m_Log.Write(_T("Coefficient is minimal!"));
	return false;
}

void CDevice::SetConnectedCircuit(const Circuit::Enum_t eCircuit)
{
	m_eCircuit = eCircuit;
}

void CDevice::RegisterOutputData(void * pData, Data::Enum_t eType, boost::mutex& mxOutData)
{
	SCOPED_LOG(m_Log);
	m_pmxOutData = &mxOutData;
	m_OutData[eType] = pData;
}

void CDevice::SendOutData()
{
	if (!m_pmxOutData)
		return;
	TRY_EXCEPTIONS
	{
		boost::mutex::scoped_lock Lock(*m_pmxOutData);
		if (m_OutData.count(Data::ePhase))
		{
			std::list< int > Tmp(1);
			Tmp.push_back(m_fPhase);
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::ePhase]) = Tmp;
		}
		if (m_OutData.count(Data::eAmplitude))
		{
			std::list< int > Tmp(1);
			Tmp.push_back(m_nAmplitude);
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::eAmplitude]) = Tmp;
		}
		if (m_OutData.count(Data::eFrequency))
			*reinterpret_cast< long *> (m_OutData[Data::eFrequency]) = m_nFrequency;
		if (m_OutData.count(Data::eIncomingSinus))
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::eIncomingSinus]) = m_InSinus;
		if (m_OutData.count(Data::eOutgoingSinus))
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::eOutgoingSinus]) = m_OutSinus;
		if (m_OutData.count(Data::eOutgoingPhase))
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::eOutgoingPhase]) = m_OutPhase;
		if (m_OutData.count(Data::eIncomingPhase))
			*reinterpret_cast< std::list< int > *> (m_OutData[Data::eIncomingPhase]) = m_InPhase;
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void DeletePointer(UCHAR * pData)
{
	delete pData;
}

void CDevice::StopWriteThread()
{
	SCOPED_LOG(m_Log);
	m_threadWrite.interrupt();
	{
		boost::mutex::scoped_lock Lock(m_mxInData);
		std::for_each(m_InData.begin(), m_InData.end(), DeletePointer);
		m_InData.clear();
	}
}

void CDevice::StopReadThread()
{
	SCOPED_LOG(m_Log);
	m_threadRead.interrupt();
	{
		boost::mutex::scoped_lock Lock(m_mxInData);
		std::for_each(m_InData.begin(), m_InData.end(), DeletePointer);
		m_InData.clear();
	}
}

void CDevice::StartWriteThread()
{
	SCOPED_LOG(m_Log);
	m_threadWrite = boost::thread(boost::bind(&CDevice::ThreadWrite, this));
}

void CDevice::StartReadThread()
{
	SCOPED_LOG(m_Log);
	m_threadRead = boost::thread(boost::bind(&CDevice::ThreadRead, this));
}

void CDevice::Disconnect()
{
	SCOPED_LOG(m_Log);
	if (m_pUsbDeviceRead)
	{
		m_pUsbDeviceRead->Close();
		m_pUsbDeviceRead.reset();
	}
	if (m_pUsbDeviceWrite)
	{
		m_pUsbDeviceWrite->Close();
		m_pUsbDeviceRead.reset();
	}
}

const int CDevice::GetDeviceCount() const
{
	SCOPED_LOG(m_Log);
	boost::scoped_ptr< CCyUSBDevice > pTempUSB(new CCyUSBDevice());
	return pTempUSB->DeviceCount();
}

const int CDevice::Connect()
{
	SCOPED_LOG(m_Log);
	boost::scoped_ptr< CCyUSBDevice > pTempUSB(new CCyUSBDevice());
	const int nCount = pTempUSB->DeviceCount();

	m_Log.Write(_tformat(_T("Device count: [%s]")) % nCount);

	for (int nDevice = 0; nDevice < nCount; ++nDevice)
	{
		// Определяем тип устройства(чтение/запись) по vendorID
		pTempUSB->Open(nDevice);
		const int nVendorID = pTempUSB->VendorID;
		pTempUSB->Close();
		pTempUSB.reset(new CCyUSBDevice());

		m_Log.Write(_tformat(_T("nVendorID: [%s]")) % nVendorID);

		//1461 - чтение
		//1463 - запись
		if (nVendorID == 1461)
		{
			m_pUsbDeviceRead.reset(new CCyUSBDevice());
			m_pUsbDeviceRead->Open(nDevice);
		}
		else if (nVendorID == 1463)
		{
			m_pUsbDeviceWrite.reset(new CCyUSBDevice());
			m_pUsbDeviceWrite->Open(nDevice);
		}
		else 
			MessageBox(0, (_tformat(_T("Unknown vendor ID [%s]!")) % nVendorID).str().c_str(), _T("cyAPI error"), MB_OK | MB_ICONERROR);
	} 

	CFile ifstr(L"Koefficients.txt", CFile::Mode::eRead);
	ifstr.Read(m_Coefficients);
	ifstr.Close();

	if (m_Coefficients.empty())
		THROW_MESSAGE("Can't load Coefficients from file!");

	if (Circuit::eNone == m_eCircuit)
		m_nCoefficient = 0.25;
	else
		m_nCoefficient = 0.25;

	SendControlBytes(m_Coefficients[m_nCoefficient]);
	return nCount;	
}

void CDevice::ClearData()
{
	m_XmlData.Childs.clear();
	m_XmlData.Data.clear();
}

void CDevice::Start(const Mode::Enum_t eMode, 
					const std::vector< CRange >& Ranges,
					TCallBack& CallBack
					)
{
	SCOPED_LOG(m_Log);

	TRY_EXCEPTIONS
	{
		if (m_bIsWorking)
			THROW_MESSAGE("Already working!")

		if (Ranges.empty())
			THROW_MESSAGE("You must set one range at least!");

		m_nFrequency = Ranges[0].Lower;
		switch (eMode)
		{
		case Mode::eSingleFrequency:
			if (Connect() != 2)
				THROW_MESSAGE("Both devices needed to single frequency measurement!");	
			m_bIsWorking = true;
			m_threadWork = boost::thread(boost::bind(&CDevice::ThreadSingle, this));
			break;		
		
		case Mode::eCalibration:
			if (Connect() != 2)
				THROW_MESSAGE("Both devices needed for calibration!");
			m_bIsWorking = true;
			SetConnectedCircuit(Circuit::eNone);
			m_threadWork = boost::thread(boost::bind(&CDevice::ThreadMultiple, this, Ranges, CallBack));
			break;
		
		case Mode::eMeasurement:
			if (Connect() != 2)
				THROW_MESSAGE("Both devices needed for measurement!");
			m_bIsWorking = true;
			SetConnectedCircuit(Circuit::eCircuit);
			m_threadWork = boost::thread(boost::bind(&CDevice::ThreadMultiple, this, Ranges, CallBack));
			break;
		
		case Mode::eReadOnly:
			if (Connect() < 1)
				THROW_MESSAGE("At least one device needed to read!");
			StartReadThread();
			break;
		
		case Mode::eWriteOnly:
			if (Connect() < 1)
				THROW_MESSAGE("At least one device needed to write!");
			StartWriteThread();
			break;
		}
	}
	CATCH_PASS_EXCEPTIONS_LOG(m_Log);
}

void CDevice::Stop()
{
	SCOPED_LOG(m_Log);

	TRY_EXCEPTIONS
	{
		StopReadThread();
		StopWriteThread();
		if (m_bIsWorking)
		{
			m_bIsWorking = false;
			m_threadWork.interrupt();
			//m_threadWork.join();
		}
	}
	CATCH_PASS_EXCEPTIONS_LOG(m_Log);
}

void CDevice::LoadAmplitudesData()
{
	SCOPED_LOG(m_Log);

	TRY_EXCEPTIONS
	{
		try
		{
			if (!m_AmplitudeXmlData.Childs.empty())
				return;

			m_Log.Write(_T("Loading amplitudes data..."));
			CXmlParser Parser;

			Parser.ParseFile(Settings::Instance().GetAmplitudesFileName());
			m_AmplitudeXmlData = Parser.GetData();

			if (m_AmplitudeXmlData.Childs.empty())
				THROW_MESSAGE("Can't load amplitudes data!");
		}
		catch (std::exception& e)
		{
			throw clrn::CMessage(e.what());
		}
	}
	CATCH_IGNORE_USER_MESSAGE_LOG(m_Log)
}

const float CDevice::GetOutAmplitude()
{
	// Находим XML узел, ближайший к m_nFrequency
	int nDelta = 1000000;
	CXmlNode::TChilds::iterator itResult = m_AmplitudeXmlData.Childs.end();
	CXmlNode::TChilds::iterator it = m_AmplitudeXmlData.Childs.begin();
	const CXmlNode::TChilds::const_iterator itEnd = m_AmplitudeXmlData.Childs.end();
	for (; it != itEnd; ++it)
	{
		const _tstring& sFrequency = it->Data[_T("frequency")];
		const int nFrequency = boost::lexical_cast< int > (sFrequency);
		const int nTempDelta = fabs(static_cast< float > (nFrequency - m_nFrequency));
		if (nTempDelta < nDelta)
		{
			nDelta = nTempDelta;
			itResult = it;
		}
	}
	const int nFrequency = boost::lexical_cast< float > (itResult->Data[_T("frequency")]);
	if (nFrequency == m_nFrequency)
	{
		return boost::lexical_cast< float > (itResult->Data[_T("amplitude_out")]);
	}
	else if (nFrequency > m_nFrequency)
	{
		const float fUpper = boost::lexical_cast< float > (itResult->Data[_T("amplitude_out")]);
		const int fUpperFreq = boost::lexical_cast< float > (itResult->Data[_T("frequency")]);

		if (itResult != m_AmplitudeXmlData.Childs.begin())
			--itResult;
		const float fLower = boost::lexical_cast< float > (itResult->Data[_T("amplitude_out")]);
		const int fLowerFreq = boost::lexical_cast< float > (itResult->Data[_T("frequency")]);

		const float fDelta = fUpper - fLower;
		const float fCoefficient = static_cast< float > (nFrequency - m_nFrequency) / (fUpperFreq - fLowerFreq);
		const float fResult = fUpper - fCoefficient * fDelta;
		return fResult;
	}
	else if (nFrequency < m_nFrequency)
	{
		const float fLower = boost::lexical_cast< float > (itResult->Data[_T("amplitude_out")]);
		const int fLowerFreq = boost::lexical_cast< float > (itResult->Data[_T("frequency")]);
		if (itResult != m_AmplitudeXmlData.Childs.end())
			++itResult;
		if (itResult == m_AmplitudeXmlData.Childs.end())
			return fLower;
		const float fUpper = boost::lexical_cast< float > (itResult->Data[_T("amplitude_out")]);
		const int fUpperFreq = boost::lexical_cast< float > (itResult->Data[_T("frequency")]);

		const float fDelta = fUpper - fLower;
		const float fCoefficient = static_cast< float > (m_nFrequency - nFrequency) / (fUpperFreq - fLowerFreq);
		const float fResult = fCoefficient * fDelta + fLower;
		return fResult;
	}
	return 0;
}

bool CDevice::CorrectInAmplitude(std::vector< int >& Data)
{
	if (Data.empty())
		return false;

	/*if (Circuit::eNone == m_eCircuit)
	{
		if (m_nCoefficient == 0.25)
			return true;
		else
		{
			StopWriteThread();
			StopReadThread();
			SendControlBytes(m_Coefficients.begin()->second);
			m_Log.Write(_tformat(_T("Calibration! Setting Coefficient from[%s] to [%s]")) % m_nCoefficient % m_Coefficients.begin()->first);
			StartReadThread();
			StartWriteThread();
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
			return false;
		}
	}*/

	unsigned __int64 nCurrentTime = boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds();

	if (nCurrentTime - m_nLastCorrectTime < 1000)
		return false;

	SCOPED_LOG(m_Log);

	// Находим средние значения полученных данных
	const int nCurrentMaximum = *std::max_element(Data.begin(), Data.end());
	const int nCurrentMinimum = *std::min_element(Data.begin(), Data.end());	

	const int nAmplitude = nCurrentMaximum - nCurrentMinimum;

	// Корректируем коэффициент усиления
	if (nAmplitude < 65 && nCurrentMaximum != 255 && nCurrentMinimum)
	{
		m_Log.Write(_tformat(_T("Amplitude [%s] Increasing Coefficient...")) % nAmplitude);
		if (IncreaseInAmplitude())
		{
			boost::mutex::scoped_lock Lock(m_mxInData);
			std::for_each(m_InData.begin(), m_InData.end(), DeletePointer);
			m_InData.clear();
			m_InSinus.clear();
			/*
			StopWriteThread();
			StartWriteThread();*/
		}
		else
		{
			m_Log.Write(_tformat(_T("Coefficient[%s] is maximal! Trying to correct out sinus.")) % m_nCoefficient);
			//CorrectOutAmplitude(m_nOutSinusAmplitude + 1);
		}

		m_nLastCorrectTime = nCurrentTime;
		return false;
	}
	else if ((nCurrentMaximum == 255 || nCurrentMinimum == 0) && nAmplitude > 50)
	{
		m_Log.Write(_tformat(_T("Amplitude [%s] Decreasing Coefficient...")) % nAmplitude);
		if (DecreaseInAmplitude())
		{
			boost::mutex::scoped_lock Lock(m_mxInData);
			std::for_each(m_InData.begin(), m_InData.end(), DeletePointer);
			m_InData.clear();
			m_InSinus.clear();
		/*
			StopWriteThread();
			StartWriteThread();*/
		}
		else
		{
			m_Log.Write(_tformat(_T("Coefficient[%s] is minimal! Trying to correct out sinus.")) % m_nCoefficient);
			//CorrectOutAmplitude(m_nOutSinusAmplitude - 1);
		}
		m_nLastCorrectTime = nCurrentTime;
		return false;
	}
	else if (nCurrentMaximum == 255 || nCurrentMinimum == 0)
	{
		m_Log.Write(_tformat(_T("Got bad sinus. K[%s] A[%s] Max[%s] Min[%s]")) % m_nCoefficient % nAmplitude % nCurrentMaximum % nCurrentMinimum);
		DecreaseInAmplitude();
		return false;
	}
	return true;
}

bool CDevice::CorrectOutAmplitude(const float nEtalonAmplitude)
{
	SCOPED_LOG(m_Log);

	m_Log.Write(_tformat(_T("Correcting out sin from [%s] to [%s].")) % m_nOutSinusAmplitude % nEtalonAmplitude);

	StopWriteThread();
	StopReadThread();
	m_nOutSinusAmplitude = nEtalonAmplitude;
	StartReadThread();
	StartWriteThread();
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	

/*
	unsigned __int64 nCurrentTime = boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds();

	// Коэффициент усиления откорректирован - корректируем исходящую амплитуду
	const int nRealAmplitude = static_cast< float > (m_nAverageAmplitude) / m_nCoefficient;

	// Определяем на сколько отклонена амплитуда
	const float nDelta = fabs(static_cast< float > (REAL_AMPLITUDE - nRealAmplitude)) / REAL_AMPLITUDE;

	// Oпределяем на сколько нужно изменить исходящую амплитуду
	float nDeltaOut = nDelta * m_nOutSinusAmplitude;

	if (REAL_AMPLITUDE - nRealAmplitude > 10)
	{
		m_nOutSinusAmplitude += nDeltaOut;
		m_Log.Write(_tformat("Real [%s] out [%s] increased by [%s]") % nRealAmplitude % m_nOutSinusAmplitude % nDeltaOut);
		StopWriteThread();
		StartWriteThread();
		m_nLastCorrectTime = nCurrentTime;
		return false;
	}
	else if (nRealAmplitude - REAL_AMPLITUDE > 20 && nRealAmplitude > 5)
	{
		m_nOutSinusAmplitude -= nDeltaOut;
		m_nOutSinusAmplitude = fabs(m_nOutSinusAmplitude);
		m_Log.Write(_tformat("Real [%s] out [%s] decreased by [%s]") % nRealAmplitude % m_nOutSinusAmplitude % nDeltaOut);
		StopWriteThread();
		StartWriteThread();
		m_nLastCorrectTime = nCurrentTime;
		return false;
	}*/
	return true;
}

void CDevice::CalcBufferSize()
{
	
	// Определяем размер буффера исходя из частоты
	const double nDt = static_cast< double > (1) / static_cast< double > (Settings::Instance().GetCAPFrequency());
	double nPeriod = 1 / (nDt * m_nFrequency) * 2;
	m_nBufferSize = nPeriod * Settings::Instance().GetPeriodsPerBuffer();
	m_nBufferSize = (m_nBufferSize / 2048) * 2048;
	if (!m_nBufferSize)
		m_nBufferSize = 2048;
	m_Log.Write(_tformat(_T("Assigned buffer to[%s] frequency[%s]")) % m_nBufferSize % m_nFrequency);
}

void CDevice::SaveData(const _tstring& sPath)
{
	SCOPED_LOG(m_Log);
	
	static std::list< float > AmplitudeIn;
	static std::list< float > Phase;
	static std::list< float > AmplitudeOut;
	AmplitudeIn.push_back(static_cast< float > (m_nAmplitude) / m_nCoefficient);
	Phase.push_back(m_fPhase);
	AmplitudeOut.push_back(m_nOutSinusAmplitude);

	if (sPath == _T("ERASE"))
	{
		AmplitudeIn.clear();
		Phase.clear();
		AmplitudeOut.clear();
	}
	else
	if (!sPath.empty())
	{
		std::list< float > NormalizedAmplitudeIn = m_Analyzer.NormalizeData(AmplitudeIn);
		std::list< float > NormalizedAmplitudeOut = m_Analyzer.NormalizeData(AmplitudeOut);
		std::list< float > NormalizedPhase = m_Analyzer.NormalizeData(Phase);

		int nSize = NormalizedAmplitudeIn.size();
		float fTemp = std::accumulate(NormalizedAmplitudeIn.begin(), NormalizedAmplitudeIn.end(), static_cast< float > (0));
		const float nAmplitudeIn = !nSize ? 0 : fTemp / nSize;
		nSize = NormalizedAmplitudeOut.size();
		fTemp = std::accumulate(NormalizedAmplitudeOut.begin(), NormalizedAmplitudeOut.end(), static_cast< float > (0));
		const int nAmplitudeOut = !nSize ? 0 : fTemp / nSize;
		nSize = NormalizedPhase.size();
		fTemp = std::accumulate(NormalizedPhase.begin(), NormalizedPhase.end(), static_cast< float > (0));
		const float fPhase = !nSize ? 0 : fTemp / nSize;

		m_Log.Write(_tformat(_T("F[%s] Ph[%s] K[%s] AI[%s] AO[%s]")) % m_nFrequency % fPhase % m_nCoefficient % nAmplitudeIn % nAmplitudeOut);
		m_Log.Write(_tformat(_T("Saving data to [%s]")) % sPath);

		CXmlNode Current;
		Current.Data[TAG_NAME]					= _T("data");
		Current.Data[_T("frequency")]			= boost::lexical_cast< _tstring > (m_nFrequency);
		Current.Data[_T("phase")]				= boost::lexical_cast< _tstring > (fPhase);
		Current.Data[_T("koeff")]				= boost::lexical_cast< _tstring > (m_nCoefficient);
		Current.Data[_T("amplitude_in")]		= boost::lexical_cast< _tstring > (nAmplitudeIn);
		Current.Data[_T("amplitude_out")]		= boost::lexical_cast< _tstring > (nAmplitudeOut);
/*
		if (Phase.size() == AmplitudeIn.size())
		{
			std::list< float >::const_iterator itPhase = Phase.begin();
			std::list< float >::const_iterator itAmplitude = AmplitudeIn.begin();
			const std::list< float >::const_iterator itPhaseEnd = Phase.end();
			for (; itPhase != itPhaseEnd; ++itPhase, ++itAmplitude)
			{
				CXmlNode TraceNode;
				TraceNode.Data[TAG_NAME]				= _T("trace_data");
				TraceNode.Data[_T("phase")]				= boost::lexical_cast< _tstring > (*itPhase);
				TraceNode.Data[_T("amplitude_in")]		= boost::lexical_cast< _tstring > (*itAmplitude);

				Current.Childs.push_back(TraceNode);
			}		
		}
*/
		AmplitudeIn.clear();
		Phase.clear();
		AmplitudeOut.clear();

		CXmlNode * pNode = m_XmlData.FindNode(_T("frequency"), boost::lexical_cast< _tstring > (m_nFrequency));
		if (pNode)
			*pNode = Current;
		else
			m_XmlData.Childs.push_back(Current);
		CXmlParser Parser;
		Parser.SetData(m_XmlData);
		Parser.SaveToFile(sPath);
	}
}

void CDevice::ThreadSingle()
{
	SCOPED_LOG(m_Log);

	m_Log.Write(_tformat(_T("Started single frequency(%s) measurement.")) % m_nFrequency);

	// Загружаем калибровочные данные и данные амлитуд
	LoadAmplitudesData();
	m_nOutSinusAmplitude = GetOutAmplitude();

	CalcBufferSize();
	StartReadThread();
	StartWriteThread();

	m_XmlData.Data[TAG_NAME]		= _T("single");
	m_XmlData.Data[_T("mode")]		= boost::lexical_cast< _tstring > (m_eCircuit);

	int nIteration = 0;
	m_nLastCorrectTime = boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds() + 1000;
	while(m_bIsWorking)
	{
		TRY_EXCEPTIONS
		{		
			// Обрабатываем полученный буфер с данными
			const bool bDataCalculated = ProcessInputData();

			if (bDataCalculated)
			{
				m_Log.Write(_tformat(_T("Iteration [%s]. Saving data.")) % ++nIteration);
				SaveData(_tstring(_T("single_")) + (Circuit::eNone == m_eCircuit ? Settings::Instance().GetCalibrationFileName() : Settings::Instance().GetMeasurementFileName()));
			}	
				
		}
		CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
	}
}

//! Потоковая ф-ция измерения диапазона частот
void CDevice::ThreadMultiple(const std::vector< CRange >& Ranges, TCallBack& CallBack)
{
	SCOPED_LOG(m_Log);

	m_nLastCorrectTime = boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds() + 1000;

	m_XmlData.Data[TAG_NAME]		= _T("multiple");
	m_XmlData.Data[_T("mode")]		= boost::lexical_cast< _tstring > (m_eCircuit);

	// Загружаем данные амлитуд
	LoadAmplitudesData();

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	m_bIsWorking = true;
	while(m_bIsWorking)
	{
		TRY_EXCEPTIONS
		{	
			std::vector< CRange >::const_iterator it = Ranges.begin();
			const std::vector< CRange >::const_iterator itEnd = Ranges.end();
			for (; it != itEnd; ++it)
			{
				for (m_nFrequency = it->Lower; m_nFrequency <= it->Upper; m_nFrequency += it->Step)
				{
					m_InSinus.clear();
					m_InPhase.clear();
					boost::this_thread::interruption_point();
					m_nOutSinusAmplitude = GetOutAmplitude();
					int nIterationCount = m_nFrequency / Settings::Instance().GetIterationCoefficient();
					if (nIterationCount < Settings::Instance().GetMinIterations())
						nIterationCount = Settings::Instance().GetMinIterations();
					if (nIterationCount > Settings::Instance().GetMaxIterations())
						nIterationCount = Settings::Instance().GetMaxIterations();
					m_Log.Write(_tformat(_T("Frequency [%s] iterations [%s].")) % m_nFrequency % nIterationCount);
					StopReadThread();
					StopWriteThread();
					CalcBufferSize();
					StartReadThread();
					StartWriteThread();

					boost::this_thread::interruption_point();

					for (int nIteration = 0; nIteration < nIterationCount; )
					{
						boost::this_thread::interruption_point();

						// Обрабатываем полученный буфер с данными
						const bool bDataCalculated = ProcessInputData();

						boost::this_thread::interruption_point();

						if (bDataCalculated)
						{
							if (!nIteration)
								m_Log.Write(_T("Starting new iterations..."));
							m_Log.WriteProgressMessage(_tformat(_T("Iteration [%s]. Saving data.")) % ++nIteration, nIteration);
							SaveData();
						}	
						else
						{
							if (nIteration)
								m_Log.WriteProgressMessage(_tformat(_T("Iteration [%s] failed.")) % nIteration, nIteration);
						}
						boost::this_thread::interruption_point();
					}
					_tstring sFileName = (Circuit::eNone == m_eCircuit ? Settings::Instance().GetCalibrationFileName() : Settings::Instance().GetMeasurementFileName());
					SaveData(sFileName);
				}
			}
			m_bIsWorking = false;
			CallBack();
		}
		CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
	}

	m_nFrequency = 0;
	m_InSinus.clear();
	m_OutSinus.clear();
	m_OutPhase.clear();
	m_InPhase.clear();
}

void CDevice::ThreadRead()
{
	SCOPED_LOG(m_Log);

	static boost::mutex mxRead;
	boost::mutex::scoped_lock Lock(mxRead);

	__int64 nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
	unsigned __int64 nTotalBytes = 0;
	UCHAR * pTemp = new UCHAR[m_nBufferSize];
	pTemp[0] = 0;
	TRY_EXCEPTIONS
	{	

		while(true)
		{
			CCyBulkEndPoint * BulkInEptIn = m_pUsbDeviceRead->BulkInEndPt;

			if (!BulkInEptIn)
				THROW_MESSAGE((boost::format("Failed to find in endpoint for device [%s]") % m_pUsbDeviceRead->FriendlyName).str().c_str());

			LONG length = m_nBufferSize;

			boost::this_thread::interruption_point();

			// Заполняем вектор с входными данными
			BulkInEptIn->XferData(pTemp, length);
			boost::this_thread::interruption_point();
			if (length != m_nBufferSize) 
				THROW_EXCEPTION("Read failed!");
			nTotalBytes += length;

			{

				boost::mutex::scoped_lock Lock(m_mxInData);
				if (m_InData.size() > 10)
				{
					std::for_each(m_InData.begin(), m_InData.end(), DeletePointer);
					m_InData.clear();
				}
				m_InData.push_back(pTemp);

				pTemp = new UCHAR[m_nBufferSize]; 
			}
			if (boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds() - nTime > 1000000)
			{
				m_nReadSpeed = nTotalBytes;
				nTotalBytes = 0;
				nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
			}
			boost::this_thread::interruption_point();
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


void CDevice::ThreadWrite()
{
	SCOPED_LOG(m_Log);

	static boost::mutex mxWrite;
	boost::mutex::scoped_lock Lock(mxWrite);
	__int64 nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
	unsigned __int64 nTotalBytes = 0;
	const double nDt = static_cast< double > (1) / static_cast< double > (Settings::Instance().GetCAPFrequency());
	double nPeriod = 1 / (nDt * m_nFrequency) * 2;

	std::vector< UCHAR > vecOutSinus(m_nBufferSize * 2);
	m_Log.Write(_T("Generating outgoing sinus..."));

	const int nStartPhase = m_Analyzer.GenerateData(m_nFrequency, vecOutSinus, m_nOutSinusAmplitude);
	{
		if (m_pmxOutData)
		{
			boost::mutex::scoped_lock Lock (*m_pmxOutData);
			m_OutSinus.clear();
			m_OutPhase.clear();

			for (int i = nStartPhase; i < nStartPhase + nPeriod; ++i)
			{
				bool bParity = i % 2;
				if (!bParity)
					m_OutSinus.push_back(static_cast< int > (vecOutSinus[i]));
			}
		}
	}
	m_Log.Write(_T("Sending..."));

	/*
	for (int i = 0; m_OutSinus.size() < m_nBufferSize && i < m_nBufferSize * 2; ++i)
	{
		bool bParity = i % 2;
		if (!bParity)
			m_OutSinus.push_back(static_cast< int > (pBuffer[i]));
		else
			m_OutPhase.push_back(static_cast< int > (pBuffer[i]) * 100);
	}*/
	
	long nCurrentStartIndex = m_nBufferSize;
	TRY_EXCEPTIONS
	{	
		while(true)
		{
			CCyBulkEndPoint * BulkInEptOut = m_pUsbDeviceWrite->BulkOutEndPt;
			if (!BulkInEptOut)
				THROW_EXCEPTION((boost::format("Failed to find out endpoint for device [%s]") % m_pUsbDeviceWrite->FriendlyName).str().c_str());

			LONG length = m_nBufferSize;

			boost::this_thread::interruption_point();
			BulkInEptOut->XferData(reinterpret_cast< UCHAR * > (&vecOutSinus[nCurrentStartIndex]), length); 
			nCurrentStartIndex = m_Analyzer.GetOutBufferIndex(m_nFrequency, nCurrentStartIndex + length, reinterpret_cast< UCHAR * > (&vecOutSinus[0]));
			
			boost::this_thread::interruption_point();
			if (length != m_nBufferSize)
				THROW_EXCEPTION("Write failed!");
			nTotalBytes += length;

			if (boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds() - nTime > 1000000)
			{
				m_nWriteSpeed = nTotalBytes;
				nTotalBytes = 0;
				nTime =  boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
			}
			boost::this_thread::interruption_point();
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


bool CDevice::ProcessInputData()
{
	SCOPED_LOG(m_Log);

	const double nDt = static_cast< double > (1) / static_cast< double > (Settings::Instance().GetCAPFrequency());
	const double nPeriod = 1 / (nDt * m_nFrequency) * 2;
	const int nPeriodsCount = 1;//PERIOD_DATA_SIZE / nPeriod + 1;
	const int nTotalPeriods = (Settings::Instance().GetPeriodsPerBuffer() - 2) * m_InData.size();
	if (nTotalPeriods < nPeriodsCount)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		return false;
	}

	// Обрабатываем полученные данные
	std::list< UCHAR * > InData;
	{
		boost::mutex::scoped_lock Lock(m_mxInData);
		InData = m_InData;
		m_InData.clear();
	}
	const int nPoints = 5000000 / m_nFrequency;

	bool bPhaseGoted = false;
	bool bIsDataCalculated = false;
	int nPhaseStartIndex = 0;
	int nPhaseEndIndex = 0;

	std::vector< int > SinusData(nPeriodsCount * m_nBufferSize / 2);
	int nPeriodsProcessed = 0;
	int nPoint = 0;
	std::list< UCHAR * >::iterator it = InData.begin();
	const std::list< UCHAR * >::const_iterator itEnd = InData.end();
	for (; it != itEnd; ++it)
	{
		boost::scoped_array< UCHAR > pCurrentBuffer(*it);
		if (!pCurrentBuffer || nPeriodsProcessed >= nPeriodsCount)
			continue;

		for (int i = 0; i < m_nBufferSize; ++i)
		{
			// Четность
			const bool bParity = i % 2;

			// Точка четная - фаза
			if (bParity)
			{
				// Определяем присутствует ли во входном буффере переход фаз
				const int nCurrentPhase = static_cast < int > (pCurrentBuffer[i]);
				if (!nPhaseStartIndex && nCurrentPhase)
					nPhaseStartIndex = i;
				if (!nCurrentPhase && !nPhaseEndIndex && nPhaseStartIndex && i > nPhaseStartIndex)
				{
					nPhaseEndIndex = i;
					//break;
				}
				if (nCurrentPhase)
					bPhaseGoted = true;
			}
			else if (bPhaseGoted && nPoint < SinusData.size())
				SinusData[nPoint++] = static_cast < int > (pCurrentBuffer[i]);
		}
		nPeriodsProcessed += nPoint * 2 / nPeriod;
	}

	// Если переход фаз присутствует - обрабатываем данные
	if (bPhaseGoted)
	{
		const int nDeltaPhase = nPhaseEndIndex - nPhaseStartIndex;

		// Определяем положение перепада фаз относительно буффера
		if (fabs(static_cast< float > (nDeltaPhase - nPeriod)) > nPeriod / 8 || nPhaseStartIndex < nPeriod || nPhaseStartIndex > m_nBufferSize - nPeriod * 2)
			return false;

		SinusData.resize(nDeltaPhase / 2);
		SinusData = m_Analyzer.Interpolate(SinusData, Settings::Instance().GetMinimumPeriodSize());

		// Корректируем амплитуды
		if (!CorrectInAmplitude(SinusData))
		{
			m_InSinus.clear();
			std::copy(SinusData.begin(), SinusData.end(), std::back_inserter(m_InSinus));
			SendOutData();
			return false;
		}

		//if (Circuit::eCircuit == m_eCircuit)
		{
			SinusData = m_Analyzer.CompressBuffer(SinusData);
			SinusData = m_Analyzer.Approximate(SinusData, SinusData.size() / (Settings::Instance().GetMinimumPeriodSize() / 25));
			SinusData = m_Analyzer.Interpolate(SinusData, Settings::Instance().GetMinimumPeriodSize() * 4);
		}

		m_InSinus.clear();
		std::copy(SinusData.begin(), SinusData.end(), std::back_inserter(m_InSinus));
		
		const int nZeroesCount = std::count(m_InSinus.begin(), m_InSinus.end(), 0);
		if (nZeroesCount > nPeriodsCount * 2)
			return false;

		int nCurrentStart = 0;
		for (int nPeriodIndex = 0; nPeriodIndex < nPeriodsCount; ++nPeriodIndex)
		{
			// Находим возрастающий ноль
			const int nStart = !nPeriodIndex ? 0 : nCurrentStart + nPeriod / 16;
			nCurrentStart = m_Analyzer.FindZero(nStart, SinusData, m_nFrequency);
			if (nPeriodIndex && nCurrentStart - nStart < nPeriod / 4)
				return false;
		}

		m_InPhase.clear();

		// Формируем импульс для обозначения начала и конца измеряемого участка синуса
		for (int i = 0; i < nCurrentStart; ++i)
			m_InPhase.push_back(255);

		m_InPhase.resize(m_InSinus.size());	

		const double dDelta = static_cast< double > (nCurrentStart) / static_cast< double > (m_InSinus.size());
		const double dDeltaCPU = static_cast< double > (11) / nPeriod;
		const int nPeriods = static_cast< int > (dDelta);
		const double dCoefficient = dDelta - nPeriods;

		const int nDeltaPhaseCPU = dDeltaCPU * 360;
		m_fPhase = dCoefficient * 360;
		//m_nPhase -= nDeltaPhaseCPU;
		if (m_fPhase >= 180)
			m_fPhase -= 358;
		const int nCurrentMaximum = *std::max_element(m_InSinus.begin(), m_InSinus.end());
		const int nCurrentMinimum = *std::min_element(m_InSinus.begin(), m_InSinus.end());

		m_nAmplitude = nCurrentMaximum - nCurrentMinimum;
		SendOutData();
		bIsDataCalculated = true;
	}
	return bIsDataCalculated;
}



};