#pragma once

// Include Project headers
#include "stdafx.h"
#include "Circle.h"
#include "XmlParser.h"
#include "Chart.h"
#include "Settings.h"

// Include windows headers
#include <Windows.h>

// Include STL headers
#include <deque>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <fstream>
#include <list>
#include <map>

// Include Boost headers
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>

//! Объект для анализа структур данных
class CAnalyzer
{
	struct Direction
	{
		enum Enum_t
		{
			eUp,
			eDown,
			eNone
		};
	};
public:

	//! Конструкторы
	CAnalyzer(const long nFrequencyACP, const long nFrequencyCAP);
	CAnalyzer();

	//! Проанализировать файл с картинкой
	void AnalizeBMP(const _tstring& sPath);

	//! Получить данные
	const std::vector< int >& GetData() const;

	//! Найти точки перелома
	const std::vector< CPOINT > GetFracturePoints();

	//! Найти точки производной
	const std::vector< int > GetDerivativePoints();

	//! Получить окружности
	std::vector< CCircle > GetCircles() const;

	//! Найти первое пересечение с нулем с указанной позиции
	const int FindZero(const int nStart, const UCHAR * pBuffer, const int nLength, const int nFrequency, const bool bParity = false);

	//! Найти первое пересечение с нулем с указанной позиции
	const int FindZero(const int nStart, std::vector< int >& Data, const int nFrequency);

	//! Сгенерировать данные для отправки
	const long GenerateData(const int nFrequency, unsigned char * pBuffer, const int nLength, const float nAmplitude);

	//! Сгенерировать данные для отправки
	const long GenerateData(const int nFrequency, std::vector< UCHAR >& Data, const float nAmplitude);

	//! Получить сдвиг фазы
	const int GetPhase(UCHAR * pInBuffer, const int nBufferSize, const int nFrequency);

	//! Получить индекс начала буфра отправки по окончанию предыдущего
	const long GetOutBufferIndex(const int nFrequency, const long LastIndex, UCHAR * pSendedBuffer);

	//! Получить амплитуду буфера
	const int GetAmplitude(UCHAR * pInBuffer, const int nLength);

	//! Получить реальную фазу :)
	const int GetRealPhase(const std::vector< int >& Data);

	//! Получить реальную фазу :)
	const float GetRealPhase(const std::list< float >& Data);

	//! Получить значения годографа из файлов
	const CChart::TData CalculateHodograph(const _tstring& sCalibrate, const _tstring& sMeasurement, const bool bShowNegativePart, std::vector< int >& Frequencies = std::vector< int >());
	
	//! Получить значения годографа из XML данных
	const CChart::TData CalculateHodograph(CXmlNode& Calibrate, CXmlNode& Measurement, const bool bShowNegativePart, std::vector< int >& Frequencies = std::vector< int >());
	
	//! Получить значения годографа из файла
	//const CChart::TData CalculateHodograph(const _tstring& sPath);
	
	//! Сжать буффер 
	const int CompressBuffer(UCHAR * pSource, const int nInLength, UCHAR * pOut, const int nOutLength);

	//! Сжать буффер
	const std::vector< int > CompressBuffer(std::vector< int >& Data);

	//! Убрать сильно отличающиеся значения из данных
	const std::list< float > NormalizeData(const std::list< float >& Data, const float fPercent = 0.2);

	//! Интерполировать данные
	const std::vector< int > Interpolate(std::vector< int >& Data, const int nMinimumSize);

	//! Аппроксимировать данные
	const CChart::TData Approximate(const CChart::TData& Input);

	//! Аппроксимировать данные
	const std::list< int > Approximate(const std::list< int >& Input);

	//! Аппроксимировать данные
	const std::vector< int > Approximate(const std::vector< int >& Input, const int nPoints = 0);

	//! Соединить XML данные
	const CXmlNode MergeXmlData(const std::vector< _tstring >& vecFiles);

	//! Получить частоту по точке годографа
	const int GetHodographFrequency(const CPoint& Point, const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies);

	//! Получить значения элементов
	const std::map< _tstring, float > CalculateElements(const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies);

	//! Получить текстовое значение элемента
	static const _tstring GetElementValue(const float fValue);

private:

	//! Данные
	std::vector< int > m_Data;

	//! Шаг для вычисления производной
	int m_nStep;

	//! Частота дискретизации АЦП
	const long m_nFreqencyACP;

	//! Частота дискретизации ЦАП
	const long m_nFreqencyCAP;
};
