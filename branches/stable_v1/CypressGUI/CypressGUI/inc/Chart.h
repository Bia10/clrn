#pragma once

// Include Project headers
#include "stdafx.h"

// Include STL headers
#include <string>
#include <list>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>

// Include Boost headers
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

//! ������ ������ � ���������
class CChart
{
public:
	CChart();
	CChart(CWnd * pWindow, const int nX, const int nY, const int nSizeX, const int nSizeY, const _tstring sXname, const _tstring sYname);
	~CChart(void);

	//! ��� ������ �������
	typedef std::list< CPoint > TData;

	//! ��� ���������� �� ����� ������� �������
	typedef std::map< COLORREF, TData > TAllData;

	//! ��������� ����� ���������
	void Start();

	//! ���������� ����� ���������
	void Stop();

	//! ���������� ���� ����
	void SetBackGroundColor(const COLORREF dwColor);

	//! �������� ������
	void ClearData();

	//! �������� ���� ������� ����� ����� � �������� ����
	const COLORREF GetActiveChartColor() const;

	//! �������� �������� ����� ����� ����� � �������� ����
	const CPoint GetActivePoint() const;

	//! �������� ������ ��� ���������
	void AddData(const COLORREF dwColor, const TData& Data);

	//! �������� ������ ��� ���������
	void AddData(const COLORREF dwColor, const std::list< int >& Data);

	//! �������� ������ ��� ���������
	void AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize);

	//! �������� ������ ��� ���������
	void AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize, bool Parity);

private:
	//! ���������� �������
	void PaintBorder();

	//! ���������� �������� ������� ��� �����
	void PaintValue();

	//! ���������������� ������
	void InitToolTip();

	//! ���������� �������� �������
	void PaintData();

	//! ��������� �-��� ��������� ������
	void PaintThread();

	//! �������� �� ������ ���� � ������� �������
	bool IsMouseInChartArea();

	//! ���������� ������ � ��������� �������
	void PaintTooltip(const CPoint& Point, const _tstring& sText);

	//! �������� ����
	CClientDC m_DC;

	//! ����� ����
	CWnd * m_hWindow;

	//! handle to the ToolTip control	
	HWND m_hWndToolTip;     

	//! ToolTip info
	TOOLINFO m_ToolTipInfo;

	//! ��������� ���������� �������
	const int m_nCornerX;
	const int m_nCornerY;

	//! ������ �� �
	const int m_nSizeX;

	//! ������ �� Y
	const int m_nSizeY;

	//! ������ ������ �� �
	static const int CELL_SIZE_X;

	//! ������ ������ �� Y
	static const int CELL_SIZE_Y;

	//! ������������ ���-�� ����������� ������
	static const int MAX_DATA_SIZE;

	//! ��� ��� �� �
	const _tstring m_sXname;

	//! ��� ��� �� Y
	const _tstring m_sYname;

	//! ������������ �� ������ � ������ ������
	bool m_bToolTipActivated;

	//! ������ �������
	TAllData m_Data;

	//! ����� ���������
	boost::thread m_threadWork;

	//! ������� �� ������ �������
	boost::mutex m_mxData;

	//! ���� ������ ������ ���������
	bool m_bIsWorking;

	//! ���� ���� �������
	COLORREF m_dwBackGroundColor;

	//! ���� ������� ����� ����� � �������� ����
	COLORREF m_dwActiveChartColor;

	//! ������� �������� ����� ����� ����� � �������� ����
	CPoint m_pointActivePoint;
};

