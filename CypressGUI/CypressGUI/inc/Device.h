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

//! Реализация USB утройства
class CDevice
{
public:
	CDevice(CLog& Log);
	~CDevice(void);

	//! Тип функции коллбэка выполнения операции
	typedef boost::function< void (void) > TCallBack;

	//! Режим работы устройства
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

	//! Тип подключенной цепи
	struct Circuit
	{
		enum Enum_t
		{
			eNone,
			eCircuit
		};
	};

	//! Типы получаемых данных
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

	//! Запустить устройство
	void										Start(const Mode::Enum_t eMode, 
													const std::vector< CRange >& Ranges,
													TCallBack& CallBack = TCallBack()
													);

	//! Остановить устройство
	void										Stop();

	//! Очистить дданыне
	void										ClearData();

	//! Установить подключенною цепь
	void										SetConnectedCircuit(const Circuit::Enum_t eCircuit);

	//! Зарегистрировать объект для получения данных
	void										RegisterOutputData(void * pData, Data::Enum_t eType, boost::mutex& mxOutData);

	//! Получить скорость чтения
	const unsigned __int64						GetReadSpeed() const;

	//! Получить скорость записи
	const unsigned __int64						GetWriteSpeed() const;

	//! Получить текущую фазу
	const float									GetPhase() const;

	//! Получить текущие XML данные
	const CXmlNode								GetData() const;

	//! Получить кол-во подключенных устр-в
	const int									GetDeviceCount() const;

private:

	//! Подключить устройства
	const int									Connect();

	//! Отключить устройства
	void										Disconnect();

	//! Потоковая ф-ция измерения на одной частоте
	void										ThreadSingle();

	//! Потоковая ф-ция измерения диапазона частот
	void										ThreadMultiple(const std::vector< CRange >& Ranges,
																TCallBack& CallBack = TCallBack()
																);
	//! Потоковая ф-ция чтения
	void										ThreadRead();

	//! Потоковая ф-ция записи
	void										ThreadWrite();

	//! Отослать управляющий байт
	void										SendControlBytes(const int nValue);

	//! Увеличить коэффициент
	bool										IncreaseInAmplitude();

	//! Уменьшить коэффициент
	bool										DecreaseInAmplitude();

	//! Обработать полученные данные
	bool										ProcessInputData();

	//! Откорректировать входящую амплитуду
	bool										CorrectInAmplitude(std::vector< int >& Data);

	//! Откорректировать исходящую амплитуду
	bool										CorrectOutAmplitude(const float nEtalonAmplitude);

	//! Сохраняем данные в файл
	void										SaveData(const _tstring& sPath = _T(""));

	//! Остановить поток записи
	void										StopWriteThread();

	//! Остановить поток чтения
	void										StopReadThread();

	//! Запустить поток записи
	void										StartWriteThread();

	//! Запустить поток чтения
	void										StartReadThread();

	//! Записать даныне в структуры, привязанные через RegisterOutputData()
	void										SendOutData();

	//! Определить размер буфера
	void										CalcBufferSize();

	//! Загрузить калибровочную информацию
	void										LoadAmplitudesData();

	//! Получить значение исходящей амплитуды
	const float									GetOutAmplitude();

	//! Ссылка на логгер
	CLog&										m_Log;

	//! Объект API контроллера на чтение
	boost::scoped_ptr< CCyUSBDevice >			m_pUsbDeviceRead;

	//! Объект API контроллера на запись
	boost::scoped_ptr< CCyUSBDevice >			m_pUsbDeviceWrite;

	//! Объект для обработки данных
	CAnalyzer									m_Analyzer;

	//! Текущая частота
	long										m_nFrequency;

	//! Текущая фаза
	float										m_fPhase;

	//! Текущая амплитуда
	int											m_nAmplitude;

	//! Текущий входящий синус
	std::list < int >							m_InSinus;

	//! Текущий исходящая фаза
	std::list < int >							m_OutPhase;

	//! Текущая входящая фаза
	std::list < int >							m_InPhase;

	//! Текущий исходящий синус
	std::list < int >							m_OutSinus;

	//! Флаг работы основного потока
	bool										m_bIsWorking;

	//! Текущий коэффициент
	float										m_nCoefficient;

	//! Текущая амплитуда генерируемого синуса
	float										m_nOutSinusAmplitude;

	//! Коэффициенты
	std::map< float, float >					m_Coefficients;

	//! Указатели на выходные данные
	std::map< Data::Enum_t, void * >			m_OutData;

	//! Указатель на мьютекс блокировки выходных данных
	boost::mutex *								m_pmxOutData;

	//! Текущие XML данные
	CXmlNode									m_XmlData;

	//! XML данные исходящих амплитуд
	CXmlNode									m_AmplitudeXmlData;

	//! Подключенная цепь
	Circuit::Enum_t								m_eCircuit;

	//! Входные данные
	std::list< UCHAR * >						m_InData;

	//! Мьютекс на входные данные
	boost::mutex								m_mxInData;

	//! Поток чтения
	boost::thread								m_threadRead;

	//! Поток записи
	boost::thread								m_threadWrite;

	//! Рабочий поток измерений
	boost::thread								m_threadWork;

	//! Скорость чтения
	unsigned __int64							m_nReadSpeed;

	//! Скорость записи
	unsigned __int64							m_nWriteSpeed;

	//! Время последней корректировки амплитуды
	unsigned __int64							m_nLastCorrectTime;

	//! Размер буфера
	int											m_nBufferSize;

};

}