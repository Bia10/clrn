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

//! ������ ��� ������� �������� ������
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

	//! ������������
	CAnalyzer(const long nFrequencyACP, const long nFrequencyCAP);
	CAnalyzer();

	//! ���������������� ���� � ���������
	void AnalizeBMP(const _tstring& sPath);

	//! �������� ������
	const std::vector< int >& GetData() const;

	//! ����� ����� ��������
	const std::vector< CPOINT > GetFracturePoints();

	//! ����� ����� �����������
	const std::vector< int > GetDerivativePoints();

	//! �������� ����������
	std::vector< CCircle > GetCircles() const;

	//! ����� ������ ����������� � ����� � ��������� �������
	const int FindZero(const int nStart, const UCHAR * pBuffer, const int nLength, const int nFrequency, const bool bParity = false);

	//! ����� ������ ����������� � ����� � ��������� �������
	const int FindZero(const int nStart, std::vector< int >& Data, const int nFrequency);

	//! ������������� ������ ��� ��������
	const long GenerateData(const int nFrequency, unsigned char * pBuffer, const int nLength, const float nAmplitude);

	//! ������������� ������ ��� ��������
	const long GenerateData(const int nFrequency, std::vector< UCHAR >& Data, const float nAmplitude);

	//! �������� ����� ����
	const int GetPhase(UCHAR * pInBuffer, const int nBufferSize, const int nFrequency);

	//! �������� ������ ������ ����� �������� �� ��������� �����������
	const long GetOutBufferIndex(const int nFrequency, const long LastIndex, UCHAR * pSendedBuffer);

	//! �������� ��������� ������
	const int GetAmplitude(UCHAR * pInBuffer, const int nLength);

	//! �������� �������� ���� :)
	const int GetRealPhase(const std::vector< int >& Data);

	//! �������� �������� ���� :)
	const float GetRealPhase(const std::list< float >& Data);

	//! �������� �������� ��������� �� ������
	const CChart::TData CalculateHodograph(const _tstring& sCalibrate, const _tstring& sMeasurement, const bool bShowNegativePart, std::vector< int >& Frequencies = std::vector< int >());
	
	//! �������� �������� ��������� �� XML ������
	const CChart::TData CalculateHodograph(CXmlNode& Calibrate, CXmlNode& Measurement, const bool bShowNegativePart, std::vector< int >& Frequencies = std::vector< int >());
	
	//! �������� �������� ��������� �� �����
	//const CChart::TData CalculateHodograph(const _tstring& sPath);
	
	//! ����� ������ 
	const int CompressBuffer(UCHAR * pSource, const int nInLength, UCHAR * pOut, const int nOutLength);

	//! ����� ������
	const std::vector< int > CompressBuffer(std::vector< int >& Data);

	//! ������ ������ ������������ �������� �� ������
	const std::list< float > NormalizeData(const std::list< float >& Data, const float fPercent = 0.2);

	//! ��������������� ������
	const std::vector< int > Interpolate(std::vector< int >& Data, const int nMinimumSize);

	//! ���������������� ������
	const CChart::TData Approximate(const CChart::TData& Input);

	//! ���������������� ������
	const std::list< int > Approximate(const std::list< int >& Input);

	//! ���������������� ������
	const std::vector< int > Approximate(const std::vector< int >& Input, const int nPoints = 0);

	//! ��������� XML ������
	const CXmlNode MergeXmlData(const std::vector< _tstring >& vecFiles);

	//! �������� ������� �� ����� ���������
	const int GetHodographFrequency(const CPoint& Point, const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies);

	//! �������� �������� ���������
	const std::map< _tstring, float > CalculateElements(const CChart::TData& Hodograph, const std::vector< int >& HodographFrequencies);

	//! �������� ��������� �������� ��������
	static const _tstring GetElementValue(const float fValue);

private:

	//! ������
	std::vector< int > m_Data;

	//! ��� ��� ���������� �����������
	int m_nStep;

	//! ������� ������������� ���
	const long m_nFreqencyACP;

	//! ������� ������������� ���
	const long m_nFreqencyCAP;
};
