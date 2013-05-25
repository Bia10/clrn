#include "StdAfx.h"
#include "DataAnalyzer.h"

#include <stdio.h>
#include <conio.h>

#include "resource.h"
#include "File.h"
#include "Settings.h"

bool Compare(std::pair< int, int >& lhs, std::pair< int, int >& rhs)
{
	return lhs.second > rhs.second;
} 

bool CompareByFreq(CXmlNode& lhs, CXmlNode& rhs)
{
	const int nLeft = boost::lexical_cast< int > (lhs.Data[_T("frequency")]);
	const int nRight = boost::lexical_cast< int > (rhs.Data[_T("frequency")]);
	return nLeft < nRight;
}

bool CompareByPos(const CPoint& lhs, const CPoint& rhs)
{
	return ((lhs.x < rhs.x) && (lhs.y < rhs.y));
}

bool CompareByX(const CPoint& lhs, const CPoint& rhs)
{
	return lhs.x < rhs.x;
}

bool CompareByY(const CPoint& lhs, const CPoint& rhs)
{
	return lhs.y > rhs.y;
}


CAnalyzer::CAnalyzer(const long nFrequencyACP, const long nFrequencyCAP) :
	m_nStep(40),
	m_nFreqencyACP(nFrequencyACP),
	m_nFreqencyCAP(nFrequencyCAP)
{

}

CAnalyzer::CAnalyzer() :
	m_nFreqencyACP(0),
	m_nFreqencyCAP(0)
{

}

void CAnalyzer::AnalizeBMP(const _tstring &sPath)
{
	CBitmap Bmp;
	HBITMAP hBmp = (HBITMAP)LoadImage(0, sPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LP_MONOCHROME);
	BOOL bMapLoaded = Bmp.Attach(hBmp);
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap* hbmSrcT = (CBitmap*)dc.SelectObject(Bmp);

	BITMAP bm;
	Bmp.GetBitmap(&bm);
	for(size_t x(0); x < bm.bmWidth; ++x)
	{
		for(size_t y(0); y < bm.bmHeight; ++y)
		{
			COLORREF color = dc.GetPixel(x ,y);
			if(color != CLR_INVALID)
			{
				const int Red   = GetRValue( color );
				const int Green = GetGValue( color );
				const int Blue  = GetBValue( color );
				if (
					Red == 0 && 
					Green == 0 && 
					Blue == 0
					)
				{
					m_Data.push_back(bm.bmHeight - y);
					break;
				}
			}
		}
	}

}

const std::vector< int >& CAnalyzer::GetData() const
{
	return m_Data;
}

const std::vector< int > CAnalyzer::GetDerivativePoints()
{
	std::vector< int > DerivativePoints;

	// Вычисляем производные с шагом m_nStep
	{
		int nLastValue = 0;
		int nCurrnetDimension = 0;
		for (int i = 0; i < m_Data.size(); i += m_nStep)
		{
			const int nCurrentValue = m_Data[i];

			const float nDerivative = (nLastValue - nCurrentValue) / (1) * -10;

			// Y - значение производной
			if (nDerivative > 0)
				DerivativePoints.push_back(nDerivative);
			else
				DerivativePoints.push_back(0);
			nLastValue = nCurrentValue;
		}
	}
	
	// Вычисляем точки перепада производной
	std::vector< int > FractureIndexes;
	{
		int nLastValue = 0;
		bool bIsFindingMax = true;
		for (int i = 0; i < DerivativePoints.size(); ++i)
		{
			const int nCurrent = DerivativePoints[i];

			const int nCurrentIndex = i > 0 ? i - 1 : 0;

			if (nCurrent < nLastValue && bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = false;
			}
			else if (!nCurrent && !bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = true;
			}
			nLastValue = nCurrent;
		}

	}
	return DerivativePoints;
	
}

const std::vector< CPOINT > CAnalyzer::GetFracturePoints()
{
	std::vector< int > DerivativePoints;

	// Вычисляем производные с шагом m_nStep
	{
		int nLastValue = 0;
		int nCurrnetDimension = 0;
		for (int i = 0; i < m_Data.size(); i += m_nStep)
		{
			const int nCurrentValue = m_Data[i];

			const float nDerivative = (nLastValue - nCurrentValue) / (1) * -10;

			// Y - значение производной
			if (nDerivative > 0)
				DerivativePoints.push_back(nDerivative);
			else
				DerivativePoints.push_back(0);
			nLastValue = nCurrentValue;
		}
	}
	std::vector< int > FractureIndexes;
	{
		int nLastValue = 0;
		bool bIsFindingMax = true;
		for (int i = 0; i < DerivativePoints.size(); ++i)
		{
			const int nCurrent = DerivativePoints[i];

			const int nCurrentIndex = i > 0 ? i - 1 : 0;

			if (nCurrent < nLastValue && bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = false;
			}
			else if (!nCurrent && !bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = true;
			}
			nLastValue = nCurrent;
		}

	}
	
	// Получаем координаты графика, соответствующие точкам перепада производных
	std::vector< POINT > FracturePoints;
	{
		for (int i = 0; i < FractureIndexes.size(); ++i)
		{
			POINT Current;
			Current.x = FractureIndexes[i];
			Current.y = m_Data[FractureIndexes[i]];
			FracturePoints.push_back(Current);
		}
	}

	std::vector< CCircle > Circles;
	// Находим средние точки на графике для вычисления окружностей
	{
		for (int i = 0; i < FracturePoints.size(); ++i)
		{
			const int nX1 = FracturePoints[i].x;
			const int nY1 = m_Data[nX1];

			++i;
			if (i >= FracturePoints.size())
				break;

			const int nX3 = FracturePoints[i].x;
			const int nY3 = m_Data[nX3];

			// Средняя точка по Х
			const int nX2 = (nX3 - nX1) / 2;
			const int nY2 = m_Data[nX2];

			CPOINT p1;
			CPOINT p2;
			CPOINT p3;

			p1.x = nX1;
			p1.y = nY1;

			p2.x = nX2;
			p3.y = nY2;

			p3.x = nX3;
			p3.y = nY3;

			CCircle C(p1, p2, p3);

			Circles.push_back(C);
		}	
	}

	// Находим точки пересечения окружностей
	std::vector< CPOINT > Result;
	{
		for (int i = 0; i < Circles.size(); ++i)
		{
			// Первая окружность
			CCircle First = Circles[i];
			++i;
			if (i >= Circles.size())
				break;

			// Вторая окружность
			CCircle Second  = Circles[i];

			std::vector< CPOINT > CrossPoints = First.GetCrossPoints(Second);
			Result.push_back(CrossPoints[0]);
			Result.push_back(CrossPoints[1]);
		}
	}

	
	return Result;
}

std::vector< CCircle > CAnalyzer::GetCircles() const
{
	std::vector< int > DerivativePoints;

	// Вычисляем производные с шагом m_nStep
	{
		int nLastValue = 0;
		int nCurrnetDimension = 0;
		for (int i = 0; i < m_Data.size(); i += m_nStep)
		{
			const int nCurrentValue = m_Data[i];

			const float nDerivative = (nLastValue - nCurrentValue) / (1) * -10;

			// Y - значение производной
			if (nDerivative > 0)
				DerivativePoints.push_back(nDerivative);
			else
				DerivativePoints.push_back(0);
			nLastValue = nCurrentValue;
		}
	}
	std::vector< int > FractureIndexes;
	{
		int nLastValue = 0;
		bool bIsFindingMax = true;
		for (int i = 0; i < DerivativePoints.size(); ++i)
		{
			const int nCurrent = DerivativePoints[i];

			const int nCurrentIndex = i > 0 ? i - 1 : 0;

			if (nCurrent < nLastValue && bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = false;
			}
			else if (nCurrent > nLastValue && !bIsFindingMax)
			{
				FractureIndexes.push_back(nCurrentIndex * m_nStep);
				bIsFindingMax = true;
			}
			nLastValue = nCurrent;
		}

	}

	// Получаем координаты графика, соответствующие точкам перепада производных
	std::vector< POINT > FracturePoints;
	{
		for (int i = 0; i < FractureIndexes.size(); ++i)
		{
			POINT Current;
			Current.x = FractureIndexes[i];
			Current.y = m_Data[FractureIndexes[i]];
			FracturePoints.push_back(Current);
		}
	}

	std::vector< CCircle > Circles;
	// Находим средние точки на графике для вычисления окружностей
	{
		for (int i = 0; i < FracturePoints.size(); ++i)
		{
			const int nX1 = FracturePoints[i].x;
			const int nY1 = m_Data[nX1];

			++i;
			if (i >= FracturePoints.size())
				break;

			const int nX3 = FracturePoints[i].x;
			const int nY3 = m_Data[nX3];

			// Средняя точка по Х
			const int nX2 = (nX3 - nX1) / 2;
			const int nY2 = m_Data[nX2];

			CPOINT p1;
			CPOINT p2;
			CPOINT p3;

			p1.x = nX1;
			p1.y = nY1;

			p2.x = nX2;
			p3.y = nY2;

			p3.x = nX3;
			p3.y = nY3;

			CCircle C(p1, p2, p3);

			Circles.push_back(C);
		}	
	}
	return Circles;
}

const int CAnalyzer::FindZero(const int nStart, const UCHAR * pBuffer, const int nLength, const int nFrequency, const bool bParity)
{
	if (nStart >= nLength)
		return nStart;

	std::list< int > Data;
	for (int i = nStart; i < nLength; ++i)
	{
		if (bParity)
		{
			bool bCurrentParity = i % 2;
			if (bCurrentParity)
				continue;
		}
		Data.push_back(static_cast< int > (pBuffer[i]));
	}

	if (Data.empty())
		return nStart;
	const int nMax = *std::max_element(Data.begin(), Data.end());
	const int nMin = *std::min_element(Data.begin(), Data.end());

	const int nMagnitude = nMax - nMin;
	const int nZero = nMagnitude / 2 + nMin;

	int nDelta = nMagnitude / 50;
	if (!nDelta)
		nDelta = 1; 

	int nStep = Settings::Instance().GetFindZeroCoefficient() / nFrequency;
	if (nStep == 0)
		nStep = 1;
	
	bool bStepParity = nStep % 2;
	if (!bStepParity)
		++nStep;

	int nLast = 0;
	for (int i = nStart; i < nLength; i += nStep)
	{
		if (bParity)
		{
			bool bCurrentParity = i % 2;
			if (bCurrentParity)
				continue;
		}
		const int nCurrent = pBuffer[i];
		if (nLast && nLast < nZero && nCurrent > nZero)
			return i - 1;
		if (nLast && nCurrent == nZero && nLast < nCurrent)
			return i;
		if (nLast == nZero && nCurrent > nLast)
			return i - 1;
		nLast = nCurrent;
	}

	return nStart;
}

const int CAnalyzer::FindZero(const int nStart, std::vector< int >& Data, const int nFrequency)
{
	if (Data.empty())
		return 0;
	const int nMax = *std::max_element(Data.begin(), Data.end());
	const int nMin = *std::min_element(Data.begin(), Data.end());

	const int nMagnitude = nMax - nMin;
	const int nZero = nMagnitude / 2 + nMin;

	int nDelta = nMagnitude / 50;
	if (!nDelta)
		nDelta = 1; 

	int nStep = Data.size() / Settings::Instance().GetFindZeroCoefficient();
	if (nStep == 0)
		nStep = 1;

	bool bStepParity = nStep % 2;
	if (!bStepParity)
		++nStep;

	int nLast = 0;
	unsigned int nCount = 0;

	for (int i = nStart; i < Data.size(); i += nStep)
	{
		bool bCurrentParity = i % 2;
		if (bCurrentParity)
			continue;
		const int nCurrent = Data[i];
		/*if (nLast && nLast < nZero && nCurrent > nZero)
			return i;*/
		if (nLast && fabs(static_cast< float > (nCurrent - nZero)) < 2 && nLast < nCurrent)
			return i + nStep;
		nLast = nCurrent;
	}

	return 0;
}
const long CAnalyzer::GenerateData(const int nFrequency, unsigned char * pOutBuffer, const int nBufferSize, const float nAmplitude)
{
	boost::scoped_array< UCHAR > pBufferSinus(new UCHAR[nBufferSize]);

	const double nDt = static_cast< double > (1) / static_cast< double > (m_nFreqencyCAP);
	const double U0 = 128;
	const double U1 = nAmplitude;//64;
	const double nPeriod = 1 / (nDt * nFrequency) * 2;

	// Сгенерировали синус
	static unsigned __int64 g_i = 0;
	for (int i = 0; i < nBufferSize; ++i)
	{ 
		pBufferSinus[i] = sin(2 * static_cast< double > (3.1415926535897932) * nFrequency * nDt * static_cast< double > (g_i)) * U1 + U0;
		++g_i;
	}

	// Определяем индекс первого максимумa
	const int nOutPhaseStartIndex = FindZero(nPeriod * 2, pBufferSinus.get(), nBufferSize, nFrequency, nBufferSize) * 2;

	// Определяем индекс второго максимума
	const int nOutPhaseEndIndex = nPeriod + nOutPhaseStartIndex;//m_Analyzer.FindZero(m_nOutPhaseStartIndex, pBufferSinus.get(), nBufferSize) * 2;

	// Заполняем буфер для передачи в контроллер:
	// нечетные значения - синус
	// четные - до nMaximumIndex - нули, после - еденицы
	for (int i = 0; i < nBufferSize; ++i)
	{
		const bool bParity = i % 2;

		if (!bParity)
		{
			// четное число - записывем значения синуса
			pOutBuffer[i] = pBufferSinus[i / 2]; 
		}
		else
		{
			// нечетное число - записываем фазу
			if (i < nOutPhaseStartIndex || i >= nOutPhaseEndIndex)
			{
				// нули
				pOutBuffer[i] = static_cast< unsigned char > (0);
			}
			else
			{
				// еденицы
				pOutBuffer[i] = static_cast< unsigned char > (255);
			}
		}
	}
	return nOutPhaseStartIndex;
}

const long CAnalyzer::GenerateData(const int nFrequency, std::vector< UCHAR >& Data, const float nAmplitude)
{
	std::vector< int > Sinus(Data.size());

	const double nDt = static_cast< double > (1) / static_cast< double > (m_nFreqencyCAP);
	const double U0 = 125;
	const double U1 = nAmplitude;//64;
	const double nPeriod = 1 / (nDt * nFrequency) * 2;

	// Сгенерировали синус
	unsigned __int64 g_i = 0;
	for (int i = 0; i < Data.size(); ++i)
	{ 
		Sinus[i] = sin(2 * static_cast< double > (3.1415926535897932) * nFrequency * nDt * static_cast< double > (g_i)) * U1 + U0;
		++g_i;
	}

	// Определяем индекс первого нуля
	const int nOutPhaseStartIndex = nPeriod * 2;

	// Определяем индекс второго нуля
	const int nOutPhaseEndIndex = nPeriod + nOutPhaseStartIndex;

	// Заполняем буфер для передачи в контроллер:
	// нечетные значения - синус
	// четные - до nMaximumIndex - нули, после - еденицы
	for (int i = 0; i < Data.size(); ++i)
	{
		const bool bParity = i % 2;

		if (!bParity)
		{
			// четное число - записывем значения синуса
			Data[i] = Sinus[i / 2]; 
		}
		else
		{
			// нечетное число - записываем фазу
			if (i < nOutPhaseStartIndex || i >= nOutPhaseEndIndex)
			{
				// нули
				Data[i] = static_cast< unsigned char > (0);
			}
			else
			{
				// еденицы
				Data[i] = static_cast< unsigned char > (255);
			}
		}
	}
	return nOutPhaseStartIndex;
}

const int CAnalyzer::GetPhase(UCHAR * pInBuffer, const int nBufferSize, const int nFrequency)
{
	// Индекс возрастающего нуля в буффере, начиная с которого берем синус
	int nInPhaseIndex = 0;
	const double nDtACP = static_cast< double > (1) / static_cast< double > (m_nFreqencyCAP);

	const double nSinPeriod = 1 / (nDtACP * nFrequency) * 2;
	for (int i = 0; i < nBufferSize; ++i)
	{
		// Четность
		const bool bParity = i % 2;

		// Точка четная - фаза
		if (bParity)
		{
			const int nCurrentPhase = static_cast< int > (pInBuffer[i]);
			if (nCurrentPhase > 0)
			{
				nInPhaseIndex = i;
				break;
			}
		}
	}

	// Определяем индекс первого возрастающего нуля начиная с индекса nInPhaseIndex
	int nFirstMaximumIndex = FindZero(nInPhaseIndex, pInBuffer, nBufferSize, nFrequency, true);

	const int nDelta = nFirstMaximumIndex - nInPhaseIndex;

	const int nDeltaPhase = static_cast< double > (nDelta) / static_cast< double > (nSinPeriod) * 360;

	return nDeltaPhase;
}

const long CAnalyzer::GetOutBufferIndex(const int nFrequency, const long LastIndex, UCHAR * pSendedBuffer)
{
	const double nDt = static_cast< double > (1) / static_cast< double > (m_nFreqencyCAP);
	const double nPeriod = 1 / (nDt * nFrequency) * 2;

	const int nDelta = LastIndex / nPeriod;
	long nResult = LastIndex - nDelta * nPeriod;
	bool bParity = nResult % 2;
	if (bParity)
		++nResult;
	return nResult;
}

const int CAnalyzer::GetAmplitude(UCHAR * pInBuffer, const int nLength)
{
	const int nMax = *std::max_element(pInBuffer, &pInBuffer[nLength]);
	const int nMin = *std::min_element(pInBuffer, &pInBuffer[nLength]);

	return nMax - nMin;
}

const int CAnalyzer::GetRealPhase(const std::vector< int >& Data)
{
	std::map< int, int > Values;
	{
		std::vector< int >::const_iterator it = Data.begin();
		const std::vector< int >::const_iterator itEnd = Data.end();
		for (; it != itEnd; ++it)
		{
			const int& nCurrent = *it;
			if (Values.count(nCurrent))
				++Values[nCurrent];
			else
				Values[nCurrent] = 1;
		}
	}
	std::vector< std::pair < int, int > > ResultValues;
	std::map< int, int >::const_iterator it = Values.begin();
	const std::map< int, int >::const_iterator itEnd = Values.end();
	for (; it != itEnd; ++it)
		ResultValues.push_back(std::pair< int, int > (it->first, it->second));

	std::sort(ResultValues.begin(), ResultValues.end(), boost::bind(&Compare, _1, _2));

	if (ResultValues.size() >= 2)
		return (ResultValues[0].first + ResultValues[1].first) / 2;
	else if (!ResultValues.empty())
		return ResultValues[0].first;

	return 0;
}

const float CAnalyzer::GetRealPhase(const std::list< float >& Data)
{
	std::map< int, int > Values;
	{
		std::list< float >::const_iterator it = Data.begin();
		const std::list< float >::const_iterator itEnd = Data.end();
		for (; it != itEnd; ++it)
		{
			const int nCurrent = *it;
			if (Values.count(nCurrent))
				++Values[nCurrent];
			else
				Values[nCurrent] = 1;
		}
	}
	std::vector< std::pair < int, int > > ResultValues;
	std::map< int, int >::const_iterator it = Values.begin();
	const std::map< int, int >::const_iterator itEnd = Values.end();
	for (; it != itEnd; ++it)
		ResultValues.push_back(std::pair< int, int > (it->first, it->second));

	std::sort(ResultValues.begin(), ResultValues.end(), boost::bind(&Compare, _1, _2));

	if (ResultValues.size() >= 2)
	{
		const float nFirst = ResultValues[0].first;
		const float nSecond = ResultValues[1].first;
		if (nFirst * nSecond > 0)
			return (nFirst + nSecond) / 2;
		else 
			return nFirst;
	}
	else if (!ResultValues.empty())
		return ResultValues[0].first;

	return 0;
}

const CChart::TData CAnalyzer::CalculateHodograph(CXmlNode& Calibrate, CXmlNode& Measurement, const bool bShowNegativePart, std::vector< int >& Frequencies)
{
	std::vector< CPoint > Result;
	std::vector< int > Phases;
	std::map< int, int > FrequenciesTmp;
	Frequencies.clear();

	unsigned const int nRShunt = Settings::Instance().GetRShunt();

	CXmlNode::TChilds::iterator it = Measurement.Childs.begin();
	const CXmlNode::TChilds::const_iterator itEnd = Measurement.Childs.end();
	for (; it != itEnd; ++it)
	{
		CXmlNode * pNode = Calibrate.FindNode(_T("frequency"), it->Data[_T("frequency")]);
		if (pNode)
		{
			const int nFrequency						= boost::lexical_cast< int > (pNode->Data[_T("frequency")]);
			const double nCalibrateAmplitude			= boost::lexical_cast< float > (pNode->Data[_T("amplitude_in")]);
			const double nCalibratePhase				= boost::lexical_cast< float > (pNode->Data[_T("phase")]);

			const double nMeasurementAmplitude			= boost::lexical_cast< float > (it->Data[_T("amplitude_in")])/* - 140*/;
			const double nMeasurementPhase				= boost::lexical_cast< float > (it->Data[_T("phase")]);

			const double Ph = nMeasurementPhase - nCalibratePhase;
			Phases.push_back(Ph);

			const float Re = (nCalibrateAmplitude * cos(Ph * 3.1415926535897932 / 180) - nMeasurementAmplitude) * nRShunt / nMeasurementAmplitude - 9900;
			const float Im = -(nCalibrateAmplitude * sin(Ph * 3.1415926535897932 / 180) * nRShunt) * 1.5 / nMeasurementAmplitude;

			if (!bShowNegativePart && (Im < 0 || Re < 0))
				continue;	
			

			{
				Result.push_back(CPoint(Re, Im));
				FrequenciesTmp[Re] = nFrequency;
			}
			
			// здесь - 9990 сопротивление параллелньо
			// в результатах убрать такой же минус
		}
	}

	std::sort(Result.begin(), Result.end(), CompareByX);

	CChart::TData Sorted;
	std::copy(Result.begin(), Result.end(), std::back_inserter(Sorted));
	{
		std::vector< CPoint >::const_iterator it = Result.begin();
		const std::vector< CPoint >::const_iterator itEnd = Result.end();
		for (; it != itEnd; ++it)
			Frequencies.push_back(FrequenciesTmp[it->x]);
	}

	return Sorted;
}

const CChart::TData CAnalyzer::CalculateHodograph(const _tstring& sCalibrate, const _tstring& sMeasurement, const bool bShowNegativePart, std::vector< int >& Frequencies)
{
	// Читаем информацию из файлов
	CXmlNode XmlCalibration;
	CXmlNode XmlMeasurement;

	{
		CXmlParser Parser;
		Parser.ParseFile(sCalibrate);
		XmlCalibration = Parser.GetData();
	}

	{
		CXmlParser Parser;
		Parser.ParseFile(sMeasurement);
		XmlMeasurement = Parser.GetData();
	}

	return CalculateHodograph(XmlCalibration, XmlMeasurement, bShowNegativePart, Frequencies);
}	

const int CAnalyzer::CompressBuffer(UCHAR * pSource, const int nInLength, UCHAR * pOut, const int nOutLength)
{
	// Определяем коэффициент сжатия
	const int nCoefficient = nInLength / nOutLength;
	int nOutIndex = 0;
	for (int i = 0; i < nInLength; i += nCoefficient)
	{
		bool bParityIn = i % 2;
		bool bParityOut = nOutIndex % 2;
		if (bParityOut)
		{
			if (bParityIn)
				pOut[nOutIndex++] = pSource[i];
			else
				pOut[nOutIndex++] = pSource[i + 1];
		}
		else
		{
			if (bParityIn)
				pOut[nOutIndex++] = pSource[i + 1];
			else
				pOut[nOutIndex++] = pSource[i];
		}
	}
	return nCoefficient;
}

const std::vector< int > CAnalyzer::CompressBuffer(std::vector< int >& Data)
{
	std::vector< int > Out;

	// Определяем коэффициент сжатия
	const int nCoefficient = Data.size() / Settings::Instance().GetMinimumPeriodSize();
	if (nCoefficient < 2)
		return Data;

	int nOutIndex = 0;
	for (int i = 0; i < Data.size(); i += nCoefficient)
		Out.push_back(Data[i]);
	return Out;
}

const std::list< float > CAnalyzer::NormalizeData(const std::list< float >& Data, const float fPercent)
{
	std::list< float > Result;
	if (Data.empty())
		return Result;

	// Определяем среднее значение
	const float fSumm = std::accumulate(Data.begin(), Data.end(), static_cast< float > (0));
	const float fAverage = fSumm / Data.size();

	// Все значения отличающиеся на fPercent отбрасываем
	std::list< float >::const_iterator it = Data.begin();
	const std::list< float >::const_iterator itEnd = Data.end();
	for (; it != itEnd; ++it)
	{
		const float fDelta = fabs(fabs(fAverage) - fabs(*it));
		if (fDelta < fabs(fAverage * fPercent))
			Result.push_back(*it);
	}

	if (Result.empty())
	{
		const float nPhase = GetRealPhase(Data);
		Result.push_back(nPhase);
	}
		
	return Result;
}	
/*
const CChart::TData CAnalyzer::CalculateHodograph(const _tstring& sPath)
{
	CXmlNode Xml;

	CXmlParser Parser;
	Parser.ParseFile(sPath);
	Xml = Parser.GetData();

	CChart::TData Result;

	unsigned const int nRShunt = Settings::Instance().GetRShunt();

	CXmlNode::TChilds::iterator it = Xml.Childs.begin();
	const CXmlNode::TChilds::const_iterator itEnd = Xml.Childs.end();
	for (; it != itEnd; ++it)
	{
		const float Ph									= boost::lexical_cast< float > (it->Data[_T("phase")]);
		const float nCalibrateAmplitude					= boost::lexical_cast< float > (it->Data[_T("calibrate")]);
		const float nMeasurementAmplitude				= boost::lexical_cast< float > (it->Data[_T("measurement")]);
	
		const float Re = (nCalibrateAmplitude * cos(Ph * 3.1415926535897932 / 180) - nMeasurementAmplitude) * nRShunt / nMeasurementAmplitude;
		const float Im = -(nCalibrateAmplitude * sin(Ph * 3.1415926535897932 / 180) * nRShunt) / nMeasurementAmplitude;

		Result.push_back(CPoint(Re, Im));
	}

	return Result;
}
*/

const std::vector< int > CAnalyzer::Interpolate(std::vector< int >& Data, const int nMinimumSize)
{
	std::vector< int > Result;

	if (Data.empty())
		return Result;
		
	if (Data.size() >= nMinimumSize)
	{
		return Data;
	}

	// Во сколько раз увеличиваем буффер
	const int nStep = nMinimumSize / Data.size();


	std::vector< int >::const_iterator it = Data.begin();
	const std::vector< int >::const_iterator itEnd = Data.end();
	for (; it != itEnd; )
	{
		const int nFirst = *it;
		++it;
		if (it == itEnd)
			break;

		const int nSecond = *it;

		// Коэффициент для значений 
		const double dCoefficient = static_cast< double > (nSecond - nFirst) / nStep;
		
		// Записываем в результирующий буффер
		for (int i = 0 ; i <= nStep; ++i)
			Result.push_back(dCoefficient * i + nFirst);

	}
	return Result;
}

const std::vector< int > CAnalyzer::Approximate(const std::vector< int >& Input, const int nPoints)
{
	std::vector< int > Result;
	if (Input.empty())
		return Result;

	for (int i = 0; i < Input.size(); i += nPoints)
	{
		if (!i)
		{
			Result.push_back(Input[i]);
			continue;
		}
		else if (i + nPoints / 2 >= Input.size())
		{
			Result.push_back(Input[Input.size() - 1]);
			continue;
		}

		std::list< float > Current;
		std::copy(&Input[i - nPoints / 2], &Input[i + nPoints / 2], std::back_inserter(Current));

		Current = NormalizeData(Current, 1.0f);

		// Сумма текущих точек
		double dCurrentSumm = std::accumulate(Current.begin(), Current.end(), static_cast< double > (0));

		const double dValue = Current.empty() ? 0 : dCurrentSumm / Current.size();

		Result.push_back(dValue);
	}
	/*
	for (int i = 0; i < Input.size(); ++i)
	{
		int nStart = i - nPoints / 2;
		int nEnd = i + nPoints / 2;
		const int nToZero = i;
		const int nToEnd = Input.size() - i;
		if (nToZero <= nPoints / 2)
		{
			Result.push_back(Input[i]);
			continue;
			nStart = 0;
			nEnd = i + 1;
		}
		if (nToEnd <= nPoints / 2 + 1)
		{
			Result.push_back(Input[i]);
			continue;
			nEnd = Input.size() - 1;
			nStart = i - (Input.size() - i);
		}

		std::list< float > Current;
		std::copy(&Input[nStart], &Input[nEnd], std::back_inserter(Current));

		Current = NormalizeData(Current, 1.0f);

		// Сумма текущих точек
		double dCurrentSumm = std::accumulate(Current.begin(), Current.end(), static_cast< double > (0));

		const double dValue = Current.empty() ? 0 : dCurrentSumm / Current.size();

		Result.push_back(dValue);
	}
*/
	return Result;
}

const std::list< int > CAnalyzer::Approximate(const std::list< int >& Input)
{
	// Кол-во точек для аппроксимации
	const int nPoints = Settings::Instance().GetApproximatePoints();

	std::list< int > Result;
	if (Input.empty())
		return Result;

	std::list< int >::const_iterator it = Input.begin();
	const std::list< int >::const_iterator itEnd = Input.end();
	for (; it != itEnd; ++it)
	{
		int nPoint = 0;

		// Находим итератор последней точки в текущей сумме
		std::list< int >::const_iterator itSumm = it;
		for (nPoint = 0; nPoint < nPoints; ++nPoint)
		{
			if (itSumm != itEnd)
				++itSumm;
			else			
				break;
		}

		std::list< float > Current;
		std::copy(it, itSumm, std::back_inserter(Current));

		Current = NormalizeData(Current, 10.5f);

		// Сумма текущих точек
		double dCurrentSumm = std::accumulate(Current.begin(), Current.end(), static_cast< double > (0));

		const double dValue = Current.empty() ? 0 : dCurrentSumm / Current.size();

		Result.push_back(dValue);
	}

	return Result;
}

const CChart::TData CAnalyzer::Approximate(const CChart::TData& Input)
{
	// Кол-во точек для аппроксимации
	const int nPoints = Settings::Instance().GetApproximatePoints();

	CChart::TData Result;
	if (Input.empty())
		return Result;
/*
	{
		std::vector< CPoint > Temp;
		std::copy(Input.begin(), Input.end(), std::back_inserter(Temp));
		std::sort(Temp.begin(), Temp.end(), CompareByPos);
		Result.push_back(*Temp.begin());
	}
	*/
	CChart::TData::const_iterator it = Input.begin();
	const CChart::TData::const_iterator itEnd = Input.end();
	for (; it != itEnd; ++it)
	{
		if (it == Input.begin())
		{
			Result.push_back(*it);
			continue;
		}

		CChart::TData::const_iterator Tempit = it;
		if (++Tempit == itEnd)
		{
			Result.push_back(*it);
			continue;
		}

		int nPoint = 0;

		// Находим итератор последней точки в текущей сумме
		CChart::TData::const_iterator itSumm = it;
		for (nPoint = 0; nPoint < nPoints; ++nPoint)
		{
			if (itSumm != itEnd)
				++itSumm;
			else			
				break;
		}

		std::list< float > CurrentX;
		std::list< float > CurrentY;
		CChart::TData::const_iterator itTemp = it;
		for (; itTemp != itSumm; ++itTemp)
		{
			CurrentX.push_back(itTemp->x);
			CurrentY.push_back(itTemp->y);
		}

		CurrentX = NormalizeData(CurrentX, 10.5f);
		CurrentY = NormalizeData(CurrentY, 10.5f);

		// Сумма текущих точек по X
		double dCurrentSummX = std::accumulate(CurrentX.begin(), CurrentX.end(), static_cast< double > (0));

		// Сумма текущих точек по Y
		double dCurrentSummY = std::accumulate(CurrentY.begin(), CurrentY.end(), static_cast< double > (0));

		const double dValueX = CurrentX.empty() ? 0 : dCurrentSummX / CurrentX.size();
		const double dValueY = CurrentY.empty() ? 0 : dCurrentSummY / CurrentY.size();

		Result.push_back(CPoint(dValueX, dValueY));
	}

	return Result;
}

const CXmlNode CAnalyzer::MergeXmlData(const std::vector< _tstring >& vecFiles)
{
	CXmlNode Result;
	Result.Data[TAG_NAME] = _T("merged_data");

	std::vector< _tstring >::const_iterator itFile = vecFiles.begin();
	const std::vector< _tstring >::const_iterator itFileEnd = vecFiles.end();
	for (; itFile != itFileEnd; ++itFile)
	{
		Result.Data[_T("files")] += *itFile;
		Result.Data[_T("files")] += _T(";");
		CXmlParser Parser;
		Parser.ParseFile(*itFile);
		CXmlNode Data = Parser.GetData();		

		CXmlNode::TChilds::iterator it = Data.Childs.begin();
		const CXmlNode::TChilds::const_iterator itEnd = Data.Childs.end();
		for (; it != itEnd; ++it)
		{
			// Определяем частоту узла
			const _tstring sFrequency = it->Data[_T("frequency")];

			// Ищем узел с такой же частотой в результате
			CXmlNode * pNode = Result.FindNode(_T("frequency"), sFrequency);

			// Если нашли - вычисляем среднее значение аттрибутов
			if (pNode)
			{
				CXmlNode::TData::iterator itNode = pNode->Data.begin();
				const CXmlNode::TData::const_iterator itNodeEnd = pNode->Data.end();
				for (; itNode != itNodeEnd; ++itNode)	
				{
					if (itNode->first != TAG_NAME)
					{
						const float fFirst = boost::lexical_cast< float > (itNode->second);
						const _tstring sValue = it->Data[itNode->first];
						if (!sValue.empty())
						{
							const float fSecond = boost::lexical_cast< float > (sValue);
							const float fAverage = (fFirst + fSecond) / 2;
							itNode->second = boost::lexical_cast< _tstring > (fAverage);
						}
						else
							itNode->second = boost::lexical_cast< _tstring > (fFirst);
					}
				}
			}
			else
				Result.Childs.push_back(*it);
		}
	}

	// Сотритруем XML данные
	std::sort(Result.Childs.begin(), Result.Childs.end(), boost::bind(&CompareByFreq, _1, _2));

	return Result;
}

const int CAnalyzer::GetHodographFrequency(const CPoint& Point, const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies)
{
	// Определяем какая по счету точка активна
	int nPoint = 0;
	CChart::TData::const_iterator it = Hodograph.begin();
	const CChart::TData::const_iterator itEnd = Hodograph.end();
	for (; it != itEnd; ++it)
	{
		if (*it == Point)
			break;
		++nPoint;
	}

	if (nPoint < HodographFrequencies.size())
		return HodographFrequencies[nPoint];
	else
		return 0;
}

const _tstring CAnalyzer::GetElementValue(const float fValue)
{
	_tstring sResult = boost::lexical_cast< _tstring > (fValue);
	const _tstring::size_type nDot = sResult.find('.');
	if (_tstring::npos != nDot)
	{
		_tstring::size_type nPower = sResult.find('e', nDot);

		if (_tstring::npos == nPower)
		{
			nPower = sResult.size() - 1;
		}

		sResult.erase(nDot + 3, nPower - nDot - 3);
	}

	return sResult;

	// Блять этот код писал с адового бодуна
	if (fValue == 0)
		return _T("0");

	if (fValue < 1)
	{
		// Определяем степень
		std::size_t nPower = 0;
		unsigned __int64 nTemp = 1;
		std::size_t nRest = fValue * nTemp;
		while(!nRest)
		{
			++nPower;
			nRest = fValue * nTemp;
			nTemp *= 10;
		}

		const float fTempValue = fValue * nTemp;

		_tstring sResult = boost::lexical_cast< _tstring > (fTempValue);
		if (sResult.size() > 4)
			sResult.resize(4);
		sResult.append(_T("E-"));
		sResult.append(boost::lexical_cast< _tstring > (nPower));
		
	}
	else
	{
		// Определяем степень
		std::size_t nPower = 0;
		unsigned __int64 nTemp = 1;
		std::size_t nRest = fValue / nTemp;
		while(nRest)
		{
			++nPower;
			nRest = fValue / nTemp;
			nTemp *= 10;
		}

		const float fTempValue = fValue * nTemp;

		_tstring sResult = boost::lexical_cast< _tstring > (fTempValue);
		if (sResult.size() > 4)
			sResult.resize(4);
		sResult.append(_T("E"));
		sResult.append(boost::lexical_cast< _tstring > (nPower));
	}

	return _T("");
}

const std::map< _tstring, float > CAnalyzer::CalculateElements(const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies)
{
	std::map< _tstring, float > Result;
	if (Hodograph.empty())
		return Result;

	const int nZeroX = 154;
	const int nZeroY = -91;

	std::vector< CPoint > vecHodograph;
	std::copy(Hodograph.begin(), Hodograph.end(), std::back_inserter(vecHodograph));

	// R0 - самая левая точка 
	std::sort(vecHodograph.begin(), vecHodograph.end(), CompareByX);
	Result[_T("R0")] = vecHodograph.begin()->x + nZeroX;

	// R1 - разница между самой правой и самой левой точкой
	std::reverse(vecHodograph.begin(), vecHodograph.end());
	Result[_T("R1")] = (vecHodograph.begin()->x - Result[_T("R0")]) * 2;

	// C1 - по формуле из точки вершины
	// Ищем самую верхнюю точку
	std::sort(vecHodograph.begin(), vecHodograph.end(), CompareByY);
	const int nRe = vecHodograph.begin()->x;
	const int nIm = vecHodograph.begin()->y;
	const CPoint Point(nRe, nIm);
	const int nFrequency = GetHodographFrequency(Point, Hodograph, HodographFrequencies);

	Result[_T("C1")] = static_cast< double > (1) / (static_cast< double > (2.0) * 3.1415926535897932f * nFrequency * (nIm + nZeroY));

	return Result;
}
