// CypressGUIDlg.h : header file
//

#pragma once

// Project files
#include "Device.h"
#include "Chart.h"
#include "TwoControl.h"
#include "OneControl.h"
#include "CircuitPage1.h"
#include "CircuitPage2.h"
#include "CircuitPage3.h"
#include "DataAnalyzer.h"
#include "File.h"
#include "XmlParser.h"
#include "Log.h"
#include "LogWnd.h"
#include "RangeDlg.h"
#include "Exception.h"
#include "Settings.h"
#include "SettingsDlg.h"

// Stl library headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
#include <math.h>

// Boost library headers
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/scoped_array.hpp>
#include <boost/function.hpp>


#include "afxwin.h"


class CCypressGUIDlgAutoProxy;


// CCypressGUIDlg dialog
class CCypressGUIDlg : public CDialog
{
	DECLARE_DYNAMIC(CCypressGUIDlg);
	friend class CCypressGUIDlgAutoProxy;

// Construction
public:
	CCypressGUIDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CCypressGUIDlg();

// Dialog Data
	enum { IDD = IDD_CYPRESSGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CCypressGUIDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// ��������� ����� 
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	//! ��������� �-��� ���������
	void ThreadPaintFun();

	//! ������� ������ ����������
	void UpdateControlValues();

	//! ���������� �������� XML �����
	void PlotXmlData(CXmlNode& Data);

	//! �����������
	void Calibration(const std::vector< CRange >& Ranges);

	//! ��������
	void Measurement(const std::vector< CRange >& Ranges);

	//! ������
	CLog									m_Log;

	//! ����� ���������
	boost::thread							m_PaintThread;

	//! ������
	boost::shared_ptr< CChart >				m_pChart;

	//! ���� � ���������� ���������
	boost::shared_ptr< CRangeDlg >			m_pRangeDlg;

	//! ������ ��� ������� ������
	CAnalyzer								m_Analyzer;

	//! ������ ����������
	boost::shared_ptr< clrn::CDevice >		m_pDevice;

	//! ������ ��� ���������� ���������
	std::map< int, std::pair< float, int > > m_HodographData;

	//! Tab ctrl for frequency
	CTabCtrl m_TabFreq;

	//! Tab ctrl for circuit
	CTabCtrl m_TabCircuit;

	//! ���� � 2�� ����������
	CTwoControl * m_pTwoControl;

	//! ���� � ����� ���������
	COneControl * m_pOneControl;

	//! �����������
	int m_nTemperature;

	//! �������� ����������
	int m_nVoltage;

	//! �������
	int m_nFrequency1;
	int m_nFrequency2;

	//! �������� ����������
	int m_nRealVoltage;

	//! �������� �������
	int m_nRealFrequency;

	//! �������� �����������
	int m_nRealTemperature;

	//! ������ ����
	int m_nGas;

	//! ������ �������
	int m_nAir;

	//! ������ �����
	int m_nMix;

	//! ������ ��������
	int m_nFlow;

	//! ��� �������
	int m_nStep;

	//! ���������� �� ����������
	bool m_bIsCalibrating;

	//! ���������� �� ���������
	bool m_bIsMeasurement;

	//! ������� ��� ������ ���������� �����������
	CButton m_CalibrateBtn;

	//! ������� ��� ������ ���������� �����������
	CButton m_MeasurementBtn;

	//! ������� ������ ��������
	CButton m_ConnectBtn;

	//! ����� ��� ��������� ����
	CBrush m_BackGroundBrush;	

	//! ������������ �� ������� ���� �� �������
	bool m_bIsPhasePloting;

	//! ������������ �� ������� �������� �� �������
	bool m_bIsHodographPloting;

	//! ���������������� �� ��������
	bool m_bIsHodographApproximating;
	
	//! ���� � ����� � ������� ����������
	_tstring m_sCalibrationFilePath;

	//! ������ ����������
	CXmlNode m_xmlCalibrationData;

	//! ������ ��� ������ �� ������
	std::list< int > m_AmplitudeIn;
	std::list< int > m_PhaseIn;
	std::list< int > m_AmplitudeOut;
	std::list< int > m_PhaseOut;

	//! ������� ��� ������ �������
	boost::mutex m_mxChartData;

	//! ����� ���������
	CChart::TData m_Hodograph;

	//! ������� ���������
	std::vector< int > m_HodographFrequencies;

	//! ������� ����� ��� ����� ���������
	int m_nHodographActiveFrequency;

	//! Re ����� ��� ����� ���������
	int m_nHodographActiveRe;

	//! Im ����� ��� ����� ���������
	int m_nHodographActiveIm;

	//! ��������� �� ������ ������� � ����� 0,0
	bool m_bIsChartPlottingFrom00;

	//! ��������� �� ������������� ����� ���������
	bool m_bIsNegativePartEnabled;

public:
	afx_msg void OnTcnSelchangeTabfreq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTabfreq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlidertemperature(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTabcircuit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTabcircuit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlidervoltageexit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlidergas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderair(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlidermix(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderflow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusIndicatorTemp();
	afx_msg void OnEnKillfocusIndicatorVoltage();
	afx_msg void OnEnKillfocusIndicatorGas();
	afx_msg void OnEnKillfocusIndicatorAir();
	afx_msg void OnEnKillfocusIndicatorMix();
	afx_msg void OnEnKillfocusIndicatorFlow();
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonCalibrate();
	afx_msg void OnBnClickedButtonMeasurement();

	afx_msg void OnDataApproximate();
	afx_msg void OnApplicationExit();
	afx_msg void OnDataSave();
	afx_msg void OnDataPlotfile();
	afx_msg void OnChartClear();
	afx_msg void OnChartStop();
	afx_msg void OnChartStart();
	afx_msg void OnChartHodohraph();
	afx_msg void OnChartPlotfile();
	afx_msg void OnLogShow();
	afx_msg void OnLogHide();
	afx_msg void OnDataMerge();
	afx_msg void OnLogClear();
	afx_msg void OnClose();
	afx_msg void OnChartShowhodohraph();
	afx_msg void OnChartShowphase();
	afx_msg void OnApplicationSettings();
	afx_msg void OnDataClear();
	afx_msg void OnChartSavehodograph();
	afx_msg void OnChartApproximatehodohraph();
	afx_msg void OnChartStartchartfrom00();
	afx_msg void OnChartShownegativepart();
};
