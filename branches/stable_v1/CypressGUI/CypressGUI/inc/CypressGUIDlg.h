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
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// получение кисти 
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	//! Потоковая ф-ция отрисовки
	void ThreadPaintFun();

	//! Считать данные контролсов
	void UpdateControlValues();

	//! Отрисовать значения XML файла
	void PlotXmlData(CXmlNode& Data);

	//! Калибровать
	void Calibration(const std::vector< CRange >& Ranges);

	//! Измерить
	void Measurement(const std::vector< CRange >& Ranges);

	//! Логгер
	CLog									m_Log;

	//! Поток отрисовки
	boost::thread							m_PaintThread;

	//! График
	boost::shared_ptr< CChart >				m_pChart;

	//! Окно с дипазонами измерений
	boost::shared_ptr< CRangeDlg >			m_pRangeDlg;

	//! Объект для анализа данных
	CAnalyzer								m_Analyzer;

	//! Объект устройства
	boost::shared_ptr< clrn::CDevice >		m_pDevice;

	//! Данные для построения годографа
	std::map< int, std::pair< float, int > > m_HodographData;

	//! Tab ctrl for frequency
	CTabCtrl m_TabFreq;

	//! Tab ctrl for circuit
	CTabCtrl m_TabCircuit;

	//! Окно с 2мя слайдерами
	CTwoControl * m_pTwoControl;

	//! Окно с одним слайдером
	COneControl * m_pOneControl;

	//! Температура
	int m_nTemperature;

	//! Выходное напряжение
	int m_nVoltage;

	//! Частоты
	int m_nFrequency1;
	int m_nFrequency2;

	//! Реальное напряжение
	int m_nRealVoltage;

	//! Реальная частота
	int m_nRealFrequency;

	//! Реальная Температура
	int m_nRealTemperature;

	//! Клапан газа
	int m_nGas;

	//! Клапан воздуха
	int m_nAir;

	//! Клапан смеси
	int m_nMix;

	//! Клапан продувки
	int m_nFlow;

	//! Шаг частоты
	int m_nStep;

	//! Проводится ли калибровка
	bool m_bIsCalibrating;

	//! Проводится ли измерение
	bool m_bIsMeasurement;

	//! Контрол для кнопки управления калибровкой
	CButton m_CalibrateBtn;

	//! Контрол для кнопки управления измерениями
	CButton m_MeasurementBtn;

	//! Контрол кнопки коннекта
	CButton m_ConnectBtn;

	//! Кисть для отрисовки фона
	CBrush m_BackGroundBrush;	

	//! Отображается ли текущая фаза на графике
	bool m_bIsPhasePloting;

	//! Отображается ли текущий годограф на графике
	bool m_bIsHodographPloting;

	//! Аппроксимируется ли годограф
	bool m_bIsHodographApproximating;
	
	//! Путь к файлу с данными калибровки
	_tstring m_sCalibrationFilePath;

	//! Данные калибровки
	CXmlNode m_xmlCalibrationData;

	//! Данные для вывода на график
	std::list< int > m_AmplitudeIn;
	std::list< int > m_PhaseIn;
	std::list< int > m_AmplitudeOut;
	std::list< int > m_PhaseOut;

	//! Мьютекс для данных графика
	boost::mutex m_mxChartData;

	//! Точки годографа
	CChart::TData m_Hodograph;

	//! Частоты годографа
	std::vector< int > m_HodographFrequencies;

	//! Частота точки под мышью годографа
	int m_nHodographActiveFrequency;

	//! Re точки под мышью годографа
	int m_nHodographActiveRe;

	//! Im точки под мышью годографа
	int m_nHodographActiveIm;

	//! выводится ли график начиная с точки 0,0
	bool m_bIsChartPlottingFrom00;

	//! Выводится ли отрицательная часть годографа
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
