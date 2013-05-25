// CypressGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "CypressGUIDlg.h"
#include "File.h"

#include <boost/filesystem.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDataSave();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCypressGUIDlg dialog




IMPLEMENT_DYNAMIC(CCypressGUIDlg, CDialog);

CCypressGUIDlg::CCypressGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCypressGUIDlg::IDD, pParent)
	, m_nTemperature(0)
	, m_nRealVoltage(0)
	, m_nRealTemperature(0)
	, m_nRealFrequency(0)
	, m_bIsCalibrating(false)
	, m_bIsMeasurement(false)
	, m_bIsPhasePloting(true)
	, m_bIsHodographPloting(false)
	, m_bIsHodographApproximating(false)
	, m_bIsChartPlottingFrom00(true)
	, m_bIsNegativePartEnabled(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;

	boost::filesystem::initial_path();
}

CCypressGUIDlg::~CCypressGUIDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
}

void CCypressGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABFREQ, m_TabFreq);
	DDX_Control(pDX, IDC_TABCIRCUIT, m_TabCircuit);
	DDX_Slider(pDX, IDC_SLIDERTEMPERATURE, m_nTemperature);
	DDX_Control(pDX, IDC_BUTTONSTART, m_CalibrateBtn);
	DDX_Control(pDX, IDC_BUTTONSTOP, m_MeasurementBtn);
	DDX_Control(pDX, IDC_BUTTONCONNECT, m_ConnectBtn);
}

BEGIN_MESSAGE_MAP(CCypressGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
//	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABFREQ, &CCypressGUIDlg::OnTcnSelchangeTabfreq)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TABFREQ, &CCypressGUIDlg::OnTcnSelchangingTabfreq)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERTEMPERATURE, &CCypressGUIDlg::OnNMCustomdrawSlidertemperature)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCIRCUIT, &CCypressGUIDlg::OnTcnSelchangeTabcircuit)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TABCIRCUIT, &CCypressGUIDlg::OnTcnSelchangingTabcircuit)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERVOLTAGEEXIT, &CCypressGUIDlg::OnNMCustomdrawSlidervoltageexit)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERGAS, &CCypressGUIDlg::OnNMCustomdrawSlidergas)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERAIR, &CCypressGUIDlg::OnNMCustomdrawSliderair)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERMIX, &CCypressGUIDlg::OnNMCustomdrawSlidermix)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERFLOW, &CCypressGUIDlg::OnNMCustomdrawSliderflow)
	ON_EN_KILLFOCUS(IDC_INDICATOR_TEMP, &CCypressGUIDlg::OnEnKillfocusIndicatorTemp)
	ON_EN_KILLFOCUS(IDC_INDICATOR_VOLTAGE, &CCypressGUIDlg::OnEnKillfocusIndicatorVoltage)
	ON_EN_KILLFOCUS(IDC_INDICATOR_GAS, &CCypressGUIDlg::OnEnKillfocusIndicatorGas)
	ON_EN_KILLFOCUS(IDC_INDICATOR_AIR, &CCypressGUIDlg::OnEnKillfocusIndicatorAir)
	ON_EN_KILLFOCUS(IDC_INDICATOR_MIX, &CCypressGUIDlg::OnEnKillfocusIndicatorMix)
	ON_EN_KILLFOCUS(IDC_INDICATOR_FLOW, &CCypressGUIDlg::OnEnKillfocusIndicatorFlow)
	ON_BN_CLICKED(IDC_BUTTONCONNECT, &CCypressGUIDlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTONSTART, &CCypressGUIDlg::OnBnClickedButtonCalibrate)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &CCypressGUIDlg::OnBnClickedButtonMeasurement)
	ON_WM_CTLCOLOR()	// требуется кисть
	ON_COMMAND(ID_SAVE_APPROXIMATE, &CCypressGUIDlg::OnDataApproximate)
	ON_COMMAND(ID_APPLICATION_EXIT, &CCypressGUIDlg::OnApplicationExit)
	ON_COMMAND(ID_DATA_SAVE32782, &CCypressGUIDlg::OnDataSave)
	ON_COMMAND(ID_CHART_CLEAR, &CCypressGUIDlg::OnChartClear)
	ON_COMMAND(ID_CHART_STOP, &CCypressGUIDlg::OnChartStop)
	ON_COMMAND(ID_CHART_START, &CCypressGUIDlg::OnChartStart)
	ON_COMMAND(ID_CHART_HODOHRAPH, &CCypressGUIDlg::OnChartHodohraph)
	ON_COMMAND(ID_CHART_PLOTFILE, &CCypressGUIDlg::OnChartPlotfile)
	ON_COMMAND(ID_LOG_SHOW, &CCypressGUIDlg::OnLogShow)
	ON_COMMAND(ID_LOG_HIDE, &CCypressGUIDlg::OnLogHide)
	ON_COMMAND(ID_DATA_MERGE, &CCypressGUIDlg::OnDataMerge)
	ON_COMMAND(ID_LOG_CLEAR, &CCypressGUIDlg::OnLogClear)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CHART_SHOWHODOHRAPH, &CCypressGUIDlg::OnChartShowhodohraph)
	ON_COMMAND(ID_CHART_SHOWPHASE, &CCypressGUIDlg::OnChartShowphase)
	ON_COMMAND(ID_APPLICATION_SETTINGS, &CCypressGUIDlg::OnApplicationSettings)
	ON_COMMAND(ID_DATA_CLEAR, &CCypressGUIDlg::OnDataClear)
	ON_COMMAND(ID_CHART_SAVEHODOGRAPH, &CCypressGUIDlg::OnChartSavehodograph)
	ON_COMMAND(ID_CHART_APPROXIMATEHODOHRAPH, &CCypressGUIDlg::OnChartApproximatehodohraph)
	ON_COMMAND(ID_CHART_STARTCHARTFROM, &CCypressGUIDlg::OnChartStartchartfrom00)
	ON_COMMAND(ID_CHART_SHOWNEGATIVEPART, &CCypressGUIDlg::OnChartShownegativepart)
END_MESSAGE_MAP()

HBRUSH CCypressGUIDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// вернуть нашу кисть
}

// CCypressGUIDlg message handlers

BOOL CCypressGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Устанавливаем размер
	CRect Rect;
	GetClientRect(&Rect);
	Rect.right = 1024;  
	Rect.bottom = 768;  
	CalcWindowRect(&Rect);
	SetWindowPos(NULL, 0, 0, Rect.Width(), Rect.Height(), SWP_NOMOVE | SWP_NOZORDER);

	srand ( time(NULL) );

	SCOPED_LOG(m_Log);

	try
	{
		Settings::Instance().Load();
	}
	catch(std::exception& e)
	{
		std::string sError = "Failed to load settings, loading default!\n";
		sError += e.what();
		MessageBoxA(0, sError.c_str(), "Error", MB_OK | MB_ICONERROR);
		Settings::Instance().LoadDefault();
	}
	// Инициализируем логгер
	m_Log.Open(clrn::CFile::GetCompletePath(Settings::Instance().GetLoggerFileName()), Settings::Instance().GetLogLevel());
	m_Log.Write(_tformat(_T("Initial path: [%s].")) % clrn::CFile::GetCompletePath(_T("")));

	m_pDevice.reset(new clrn::CDevice(m_Log));

	m_pDevice->RegisterOutputData(&m_AmplitudeIn, clrn::CDevice::Data::eIncomingSinus, m_mxChartData);
	m_pDevice->RegisterOutputData(&m_AmplitudeOut, clrn::CDevice::Data::eOutgoingSinus, m_mxChartData);	
	m_pDevice->RegisterOutputData(&m_PhaseOut, clrn::CDevice::Data::eOutgoingPhase, m_mxChartData);
	m_pDevice->RegisterOutputData(&m_PhaseIn, clrn::CDevice::Data::eIncomingPhase, m_mxChartData);

	//m_TabFreq
	// Добавляем вкладки для таб-контрола частоты
	{

		TC_ITEM tci;
		tci.mask = TCIF_TEXT;
		tci.iImage = -1;

		tci.pszText = _T("f1..f2");
		m_TabFreq.InsertItem(0, &tci);
		tci.pszText = _T("f");
		m_TabFreq.InsertItem(1, &tci);

		TC_ITEM TabItem;
		CTwoControl* pPage1 = new CTwoControl;
		m_pTwoControl = pPage1;
		TabItem.mask = TCIF_PARAM;
		TabItem.lParam = (LPARAM)pPage1;
		m_TabFreq.SetItem(0, &TabItem);
		VERIFY(pPage1->Create(CTwoControl::IDD, &m_TabFreq));
		pPage1->SetWindowPos(NULL, 0, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pPage1->ShowWindow(SW_SHOW);

		COneControl* pPage2 = new COneControl;
		m_pOneControl = pPage2;
		TabItem.mask = TCIF_PARAM;
		TabItem.lParam = (LPARAM)pPage2;
		m_TabFreq.SetItem(1, &TabItem);
		VERIFY(pPage2->Create(COneControl::IDD, &m_TabFreq));
		pPage2->SetWindowPos(NULL, 0, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pPage2->ShowWindow(SW_HIDE);
	}

	//m_TabCircuit.
	// Добавляем вкладки для таб-контрола со схемами
	{
		TC_ITEM tci;
		tci.mask = TCIF_TEXT;
		tci.iImage = -1;

		tci.pszText = _T("1");
		m_TabCircuit.InsertItem(0, &tci);
		tci.pszText = _T("2");
		m_TabCircuit.InsertItem(1, &tci);		
		tci.pszText = _T("3");
		m_TabCircuit.InsertItem(2, &tci);

		TC_ITEM TabItem;
		CCircuitPage1* pPage1 = new CCircuitPage1;
		TabItem.mask = TCIF_PARAM;
		TabItem.lParam = (LPARAM)pPage1;
		m_TabCircuit.SetItem(0, &TabItem);
		VERIFY(pPage1->Create(CCircuitPage1::IDD, &m_TabCircuit));
		pPage1->SetWindowPos(NULL, 0, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pPage1->ShowWindow(SW_SHOW);

		CCircuitPage2* pPage2 = new CCircuitPage2;
		TabItem.mask = TCIF_PARAM;
		TabItem.lParam = (LPARAM)pPage2;
		m_TabCircuit.SetItem(1, &TabItem);
		VERIFY(pPage2->Create(CCircuitPage2::IDD, &m_TabCircuit));
		pPage2->SetWindowPos(NULL, 0, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pPage2->ShowWindow(SW_HIDE);

		CCircuitPage3* pPage3 = new CCircuitPage3;
		TabItem.mask = TCIF_PARAM;
		TabItem.lParam = (LPARAM)pPage3;
		m_TabCircuit.SetItem(2, &TabItem);
		VERIFY(pPage3->Create(CCircuitPage3::IDD, &m_TabCircuit));
		pPage3->SetWindowPos(NULL, 0, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pPage3->ShowWindow(SW_HIDE);

	}

	// USB init
	//OnBnClickedButtonconnect();

	CRangeDlg::TUpdateFun Update(boost::bind(&CCypressGUIDlg::UpdateControlValues, this));

	// Range dialog
	m_pRangeDlg.reset(
		new CRangeDlg(
			m_nFrequency1,
			m_nFrequency2,
			m_nStep,
			Update
		)
	);

	// Chart init
	m_pChart.reset(new 
		CChart(this, 
			635,			// угол Х
			80,				// угол Y
			370,			// Размер по Х
			290,			// Размер по Y
			_T("Time"), 
			_T("Voltage")
			)
	);
	m_pChart->SetBackGroundColor(RGB(255, 255, 255));
	m_pChart->Start();

	// Paint thread
	m_PaintThread = boost::thread(boost::bind(&CCypressGUIDlg::ThreadPaintFun, this));

	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));

	m_Analyzer.GetElementValue(0.0000001);
	m_Analyzer.GetElementValue(0.0000003211);
	m_Analyzer.GetElementValue(0);
	m_Analyzer.GetElementValue(10000000000000000);
	m_Analyzer.GetElementValue(13210000000000000000);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCypressGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCypressGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCypressGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.


void CCypressGUIDlg::OnOK()
{
	if (CanExit())
		CDialog::OnOK();
}

void CCypressGUIDlg::OnCancel()
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CCypressGUIDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}


void CCypressGUIDlg::OnTcnSelchangeTabfreq(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nTab = m_TabFreq.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_TabFreq.GetItem(nTab, &tci);
	ASSERT(tci.lParam);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_SHOW);

}

void CCypressGUIDlg::OnTcnSelchangingTabfreq(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int nTab = m_TabFreq.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_TabFreq.GetItem(nTab, &tci);
	ASSERT(tci.lParam);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_HIDE);
}

void CCypressGUIDlg::OnTcnSelchangeTabcircuit(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nTab = m_TabCircuit.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_TabCircuit.GetItem(nTab, &tci);
	ASSERT(tci.lParam);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_SHOW);
}

void CCypressGUIDlg::OnTcnSelchangingTabcircuit(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int nTab = m_TabCircuit.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_TabCircuit.GetItem(nTab, &tci);
	ASSERT(tci.lParam);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_HIDE);
}

void CCypressGUIDlg::UpdateControlValues()
{
	// Таб контрол со слайдерами
	{
		int nTab = m_TabFreq.GetCurSel();
		if (nTab == 0)
		{
			m_nFrequency1	= m_pTwoControl->GetLowerFrequency();
			m_nFrequency2	= m_pTwoControl->GetUpperFrequency();
			m_nStep			= m_pTwoControl->GetStep();
		}
		else
		{
			m_nFrequency1	= m_pOneControl->GetFrequency();
			m_nFrequency2	= 0;
			m_nStep			= 0;
		}
	}

	// Реальные значения
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDITTEMPERATURE));
		const std::wstring sValue = boost::lexical_cast< std::wstring > (m_nRealTemperature);
		EditBox->SetWindowText(sValue.c_str());
	}

	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDITVOLTAGE));
		const std::wstring sValue = boost::lexical_cast< std::wstring > (m_nRealVoltage);
		EditBox->SetWindowText(sValue.c_str());
	}

	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDITFREQ));
		const std::wstring sValue = boost::lexical_cast< std::wstring > (m_nRealFrequency);
		EditBox->SetWindowText(sValue.c_str());
	}

	// Определяем выделен ли годограф мышью
	const COLORREF dwCurrentChart = m_pChart->GetActiveChartColor();
	if ((RGB(0, 0, 0) == dwCurrentChart || RGB(1, 1, 1) == dwCurrentChart) && !m_HodographFrequencies.empty())
	{
		const CPoint pointClosest = m_pChart->GetActivePoint();
		m_nHodographActiveRe = pointClosest.x;
		m_nHodographActiveIm = pointClosest.y;

		m_nHodographActiveFrequency = m_Analyzer.GetHodographFrequency(pointClosest, m_Hodograph, m_HodographFrequencies);
	}
	else
	{
		m_nHodographActiveRe = 0;
		m_nHodographActiveIm = 0;
		m_nHodographActiveFrequency = 0;
	}

	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_HODOGRAPH_FREQUENCY));
		const _tstring sValue = boost::lexical_cast< _tstring > (m_nHodographActiveFrequency);
		EditBox->SetWindowText(sValue.c_str());
	}
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_Z));
		const _tstring sValue = boost::lexical_cast< _tstring > (m_nHodographActiveRe);
		EditBox->SetWindowText(sValue.c_str());
	}
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_FI));
		const _tstring sValue = boost::lexical_cast< _tstring > (m_nHodographActiveIm);
		EditBox->SetWindowText(sValue.c_str());
	}

	std::map< _tstring, float > Elements = m_Analyzer.CalculateElements(m_Hodograph, m_HodographFrequencies);
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_R0));
		const _tstring sValue = m_Analyzer.GetElementValue(Elements[_T("R0")]);
		EditBox->SetWindowText(sValue.c_str());
	}
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_R1));
		const _tstring sValue = m_Analyzer.GetElementValue(Elements[_T("R1")]);
		EditBox->SetWindowText(sValue.c_str());
	}
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_C1));
		const _tstring sValue = m_Analyzer.GetElementValue(Elements[_T("C1")]);
		EditBox->SetWindowText(sValue.c_str());
	}
}

void CCypressGUIDlg::OnNMCustomdrawSlidertemperature(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
		
	// 1-й слайдер
	{
		CSliderCtrl *SliderTemperature;
		SliderTemperature = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERTEMPERATURE));
		SliderTemperature->SetRange(0, 500);
		m_nTemperature = SliderTemperature->GetPos();
	}
	// Устанавливаем значения индикаторов
	// Температура
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_TEMP));
		const std::wstring sValue = boost::lexical_cast< std::wstring > (500 - m_nTemperature);
		EditBox->SetWindowText(sValue.c_str());
	}
}


void CCypressGUIDlg::OnNMCustomdrawSlidervoltageexit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// 2-й слайдер
	{
		CSliderCtrl *SliderVoltage;
		SliderVoltage = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERVOLTAGEEXIT));
		SliderVoltage->SetRange(0, 1000);
		m_nVoltage = SliderVoltage->GetPos();
	}

	// напряжение
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_VOLTAGE));
		const std::wstring sValue = boost::lexical_cast< std::wstring > (1000 - m_nVoltage);
		EditBox->SetWindowText(sValue.c_str());
	}

}

void CCypressGUIDlg::OnNMCustomdrawSlidergas(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	// Управление установкой
	// газ
	{
		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERGAS));
		Slider->SetRange(0, 60);
		m_nGas = Slider->GetPos();
	}
	// Газ
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_GAS));
		const std::wstring sValue = boost::lexical_cast< std::wstring > ((60 - m_nGas));
		EditBox->SetWindowText(sValue.c_str());
	}
}

void CCypressGUIDlg::OnNMCustomdrawSliderair(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// воздух
	{
		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERAIR));
		Slider->SetRange(0, 60);
		m_nAir = Slider->GetPos();
	}	
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_AIR));
		const std::wstring sValue = boost::lexical_cast< std::wstring > ((60 - m_nAir));
		EditBox->SetWindowText(sValue.c_str());
	}
}


void CCypressGUIDlg::OnNMCustomdrawSlidermix(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	// смесь
	{
		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERMIX));
		Slider->SetRange(0, 60);
		m_nMix = Slider->GetPos();
	}

	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_MIX));
		const std::wstring sValue = boost::lexical_cast< std::wstring > ((60 - m_nMix));
		EditBox->SetWindowText(sValue.c_str());
	}

}

void CCypressGUIDlg::OnNMCustomdrawSliderflow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// продувка
	{
		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERFLOW));
		Slider->SetRange(0, 60);
		m_nFlow = Slider->GetPos();
	}

	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_FLOW));
		const std::wstring sValue = boost::lexical_cast< std::wstring > ((60 - m_nFlow));
		EditBox->SetWindowText(sValue.c_str());
	}
}

void CCypressGUIDlg::OnEnKillfocusIndicatorTemp()
{
	// TODO: Add your control notification handler code here
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_TEMP));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nTemperature = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *SliderTemperature;
	SliderTemperature = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERTEMPERATURE));
	SliderTemperature->SetPos((500 - m_nTemperature));
	const int nTest = SliderTemperature->GetPos();
}

void CCypressGUIDlg::OnEnKillfocusIndicatorVoltage()
{
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_VOLTAGE));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nVoltage = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERVOLTAGEEXIT));
	Slider->SetPos((1000 - m_nVoltage));
	const int nTest = Slider->GetPos();
}

void CCypressGUIDlg::OnEnKillfocusIndicatorGas()
{
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_GAS));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nGas = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERGAS));
	Slider->SetPos((60 - m_nGas));
	const int nTest = Slider->GetPos();
}

void CCypressGUIDlg::OnEnKillfocusIndicatorAir()
{
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_AIR));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nAir = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERAIR));
	Slider->SetPos((60 - m_nAir));
	const int nTest = Slider->GetPos();
}

void CCypressGUIDlg::OnEnKillfocusIndicatorMix()
{
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_MIX));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nMix = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERMIX));
	Slider->SetPos((60 - m_nMix));
	const int nTest = Slider->GetPos();
}

void CCypressGUIDlg::OnEnKillfocusIndicatorFlow()
{
	CEdit * EditBox;
	EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_FLOW));

	CString sValue;
	EditBox->GetWindowText(sValue);

	m_nFlow = boost::lexical_cast< int > (sValue.GetBuffer());

	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERFLOW));
	Slider->SetPos((60 - m_nFlow));
	const int nTest = Slider->GetPos();
}

void CCypressGUIDlg::OnBnClickedButtonconnect()
{
	SCOPED_LOG(m_Log);

	TRY_EXCEPTIONS
	{
		MessageBox(_T("Connect only read device and press OK."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		if (m_pDevice->GetDeviceCount() == 2)
			THROW_MESSAGE("Both devices connected!");
		if (m_pDevice->GetDeviceCount() == 0)
			THROW_MESSAGE("Can't find device!");
		m_pDevice->SetConnectedCircuit(clrn::CDevice::Circuit::eNone);
		std::vector< CRange > Ranges;
		CRange Range;
		Range.Lower = 1000;
		Range.Upper = 0;
		Range.Step = 0;
		Ranges.push_back(Range);
		m_pDevice->Start(clrn::CDevice::Mode::eReadOnly, Ranges);
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		MessageBox(_T("Now you can connect write device"), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		m_pDevice->Stop();
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


void CCypressGUIDlg::OnBnClickedButtonCalibrate()
{	
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		if (m_bIsMeasurement)
			THROW_MESSAGE("Stop measurement first!");

		if (m_bIsCalibrating)
		{
			m_bIsCalibrating = false;
			m_pDevice->Stop();
			m_CalibrateBtn.SetWindowText(_T("Calibrate"));
			return;
		}
		UpdateControlValues();

		int nTab = m_TabFreq.GetCurSel();
		if (nTab == 0)
		{
			CRangeDlg::TCallBackFun CallBack(boost::bind(&CCypressGUIDlg::Calibration, this, _1));
			m_pRangeDlg->Init(CallBack);
			m_pRangeDlg->ShowWindow(SW_SHOWNORMAL);
			m_pRangeDlg->UpdateWindow();
		}
		else
		{
			std::vector< CRange > Ranges;
			CRange Range;
			Range.Lower = m_nFrequency1;
			Range.Upper = 0;
			Range.Step = 0;
			Ranges.push_back(Range);
			Calibration(Ranges);
		}

	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::OnBnClickedButtonMeasurement()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		if (m_bIsCalibrating)
			THROW_MESSAGE("Stop calibration first!");

		if (m_bIsMeasurement)
		{
			m_bIsMeasurement = false;
			m_pDevice->Stop();
			m_MeasurementBtn.SetWindowText(_T("Measurement"));
			return;
		}

		UpdateControlValues();
		int nTab = m_TabFreq.GetCurSel();
		if (nTab == 0)
		{
			CRangeDlg::TCallBackFun CallBack(boost::bind(&CCypressGUIDlg::Measurement, this, _1));
			m_pRangeDlg->Init(CallBack);
			m_pRangeDlg->ShowWindow(SW_SHOWNORMAL);
			m_pRangeDlg->UpdateWindow();
		}
		else
		{
			std::vector< CRange > Ranges;
			CRange Range;
			Range.Lower = m_nFrequency1;
			Range.Upper = 0;
			Range.Step = 0;
			Ranges.push_back(Range);
			Measurement(Ranges);
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::Calibration(const std::vector< CRange >& Ranges)
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		if (!m_bIsCalibrating)
		{
			clrn::CDevice::TCallBack Callback(boost::bind(&CCypressGUIDlg::Calibration, this, Ranges));

			m_pDevice->SetConnectedCircuit(clrn::CDevice::Circuit::eNone);

			// Определяем какая вкладка с частотами активна
			int nTab = m_TabFreq.GetCurSel();
			if (nTab == 0)
			{
				// Активна вкладка с диапазоном частот
				m_pDevice->Start(clrn::CDevice::Mode::eCalibration, Ranges, Callback);
			}
			else
				m_pDevice->Start(clrn::CDevice::Mode::eSingleFrequency, Ranges);

			m_bIsCalibrating = true;
			m_CalibrateBtn.SetWindowText(_T("Stop"));
		}
		else
		{
			m_bIsCalibrating = false;
			m_pDevice->Stop();
			MessageBox(_T("Job comleted!"), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
			m_CalibrateBtn.SetWindowText(_T("Calibrate"));
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


void CCypressGUIDlg::Measurement(const std::vector< CRange >& Ranges)
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		UpdateControlValues();

		if (!m_bIsMeasurement)
		{
			clrn::CDevice::TCallBack Callback(boost::bind(&CCypressGUIDlg::Measurement, this, Ranges));
			m_pDevice->SetConnectedCircuit(clrn::CDevice::Circuit::eCircuit);

			// Определяем какая вкладка с частотами активна
			int nTab = m_TabFreq.GetCurSel();
			if (nTab == 0)
			{
				// Активна вкладка с диапазоном частот
				m_pDevice->Start(clrn::CDevice::Mode::eMeasurement, Ranges, Callback);
			}
			else
				m_pDevice->Start(clrn::CDevice::Mode::eSingleFrequency, Ranges);

			m_bIsMeasurement = true;
			m_MeasurementBtn.SetWindowText(_T("Stop"));
		}
		else
		{
			m_bIsMeasurement = false;
			m_pDevice->Stop();
			MessageBox(_T("Job comleted!"), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
			m_MeasurementBtn.SetWindowText(_T("Measurement"));
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::ThreadPaintFun()
{
	SCOPED_LOG(m_Log);

	TRY_EXCEPTIONS
	{
		while(true)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(200));
			if (m_bIsPhasePloting)
			{
				boost::mutex::scoped_lock Lock(m_mxChartData);

				m_pChart->AddData(RGB(0, 0, 255), m_AmplitudeIn);
				m_pChart->AddData(RGB(255, 0, 0), m_PhaseIn);
				m_pChart->AddData(RGB(0, 255, 0), m_AmplitudeOut);
				m_pChart->AddData(RGB(254, 0, 0), m_PhaseOut);
			}
			else
			{
				m_pChart->AddData(RGB(0, 0, 255), std::list< int > ());
				m_pChart->AddData(RGB(255, 0, 0), std::list< int > ());
				m_pChart->AddData(RGB(0, 255, 0), std::list< int > ());
				m_pChart->AddData(RGB(254, 0, 0), std::list< int > ());
			}
			if (m_bIsHodographPloting)
			{
				if (!m_sCalibrationFilePath.empty())
				{
					if (m_xmlCalibrationData.Childs.empty())
					{
						CXmlParser Parser;
						Parser.ParseFile(clrn::CFile::GetCompletePath(m_sCalibrationFilePath));
						m_xmlCalibrationData = Parser.GetData();
					}

					CXmlNode xmlData = m_pDevice->GetData();

					m_Hodograph = m_Analyzer.CalculateHodograph(m_xmlCalibrationData, xmlData, m_bIsNegativePartEnabled, m_HodographFrequencies);
					if (m_bIsHodographApproximating)
					{
						m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
						m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
						m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
					}

					if (m_bIsChartPlottingFrom00)
						m_Hodograph.push_front(CPoint(0, 0));

					m_pChart->AddData(RGB(0, 0, 0), m_Hodograph);
				}
			}
			else
			{
				m_pChart->AddData(RGB(0, 0, 0), std::list< int > ());
			}
			m_nRealFrequency = m_pDevice->GetReadSpeed();
			m_nRealTemperature = m_pDevice->GetWriteSpeed();
			m_nRealVoltage = m_pDevice->GetPhase();

			UpdateControlValues();	
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


void CCypressGUIDlg::OnDataApproximate()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

		CXmlNode Data;
		{
			CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

				CXmlParser Parser;
				Parser.ParseFile(clrn::CFile::GetCompletePath(sPath));
				Data = Parser.GetData();
			}
			else
				return;	
		}

		CChart::TData Phase;
		CChart::TData Amplitude;
		CChart::TData Hodograph;


		{
			CXmlNode::TChilds::iterator it = Data.Childs.begin();
			const CXmlNode::TChilds::const_iterator itEnd = Data.Childs.end();
			for (; it != itEnd; ++it)
			{
				TRY_EXCEPTIONS
				{
					const int nX = boost::lexical_cast< float > (it->Data[_T("frequency")]);
					int nY = boost::lexical_cast< float > (it->Data[_T("phase")]);
					if (nY > 180)
						nY -= 358;

					Phase.push_back(CPoint(nX, nY));	
				}
				CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);

				TRY_EXCEPTIONS
				{
					const int nX = boost::lexical_cast< float > (it->Data[_T("frequency")]);
					const int nY = boost::lexical_cast< float > (it->Data[_T("amplitude_in")]);
					Amplitude.push_back(CPoint(nX, nY));	
				}
				CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);

				TRY_EXCEPTIONS
				{
					const int nX = boost::lexical_cast< float > (it->Data[_T("re")]);
					const int nY = boost::lexical_cast< float > (it->Data[_T("im")]);
					Hodograph.push_back(CPoint(nX, nY));	
				}
				CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
			}
		}

	
		Amplitude	= m_Analyzer.Approximate(Amplitude);
		Phase		= m_Analyzer.Approximate(Phase);
		Hodograph	= m_Analyzer.Approximate(Hodograph);

		if (Amplitude.size() != Phase.size())
			THROW_MESSAGE("Amplitudes and phases got different size!");

		CXmlNode Approximated;
		Approximated.Data = Data.Data;

		CChart::TData::const_iterator itA = Amplitude.begin();
		CChart::TData::const_iterator itP = Phase.begin();
		const CChart::TData::const_iterator itEnd = Amplitude.end();
		for (; itA != itEnd; ++itA, ++itP)
		{
			CXmlNode Current;
			Current.Data[TAG_NAME]					= _T("data");
			Current.Data[_T("frequency")]			= boost::lexical_cast< _tstring > (itA->x);
			Current.Data[_T("phase")]				= boost::lexical_cast< _tstring > (itP->y);
			Current.Data[_T("amplitude_in")]		= boost::lexical_cast< _tstring > (itA->y);

			Approximated.Childs.push_back(Current);
		}

		CChart::TData::const_iterator itH = Hodograph.begin();
		const CChart::TData::const_iterator itHEnd = Hodograph.end();
		for (; itH != itHEnd; ++itH)
		{
			CXmlNode Current;
			Current.Data[TAG_NAME]					= _T("point");
			Current.Data[_T("re")]					= boost::lexical_cast< _tstring > (itH->x);
			Current.Data[_T("im")]					= boost::lexical_cast< _tstring > (itH->y);

			Approximated.Childs.push_back(Current);
		}

		{
			CFileDialog FileDlg(FALSE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

				CXmlParser Parser;
				Parser.SetData(Approximated);
				Parser.SaveToFile(clrn::CFile::GetCompletePath(sPath));
			}
			else
				return;	
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}


void CCypressGUIDlg::OnApplicationExit()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		m_pDevice->Stop();
		exit(0);	
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::OnDataSave()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		CXmlNode Data = m_pDevice->GetData();
		if (Data.Childs.empty())
		{
			MessageBox(_T("Nothing to save!"), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
			return;
		}

		LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

		CFileDialog FileDlg(FALSE, _T(".xml"), NULL, 0, strFilter, this);
		if(FileDlg.DoModal() == IDOK)
		{
			POSITION fileNamesPosition = FileDlg.GetStartPosition();
			const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

			CXmlParser Parser;

			Parser.SetData(Data);
			Parser.SaveToFile(clrn::CFile::GetCompletePath(sPath));
		}
		else
			return;
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::PlotXmlData(CXmlNode& Data)
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		CXmlNode * pNode = Data.FindNode(TAG_NAME, _T("Hodograph"));
		if (!pNode)
		{
			CChart::TData Amplitude;
			CChart::TData Phase;
			Phase.push_back(CPoint(0, 360));

			CXmlNode::TChilds::iterator it = Data.Childs.begin();
			const CXmlNode::TChilds::iterator itEnd = Data.Childs.end();
			for (; it != itEnd; ++it)
			{
				const int nFrequency = boost::lexical_cast< int > (it->Data[_T("frequency")]);
				TRY_EXCEPTIONS
				{
					const int nAmplitude = boost::lexical_cast< int > (it->Data[_T("amplitude_in")]);
					Amplitude.push_back(CPoint(nFrequency, nAmplitude));
				}
				CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
				TRY_EXCEPTIONS
				{
					float fPhase = boost::lexical_cast< float > (it->Data[_T("phase")]);
					const float fToMinus = fPhase + 180;
					const float fToZero = fabs(fPhase);
					if (fToMinus < fToZero)
						fPhase += 360;
					Phase.push_back(CPoint(nFrequency, fPhase));
				}
				CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
			}

			if (!Amplitude.empty())
				m_pChart->AddData(RGB(255, 0, 0), Amplitude);

			if (!Phase.empty())
				m_pChart->AddData(RGB(0, 0, 255), Phase);
		}
		else
		{
			CChart::TData Hodograph;
			CXmlNode::TChilds::iterator it = Data.Childs.begin();
			const CXmlNode::TChilds::iterator itEnd = Data.Childs.end();
			for (; it != itEnd; ++it)
			{
				const int nRe = boost::lexical_cast< int > (it->Data[_T("re")]);
				const int nIm = boost::lexical_cast< int > (it->Data[_T("im")]);

				Hodograph.push_back(CPoint(nRe, nIm));
			}

			if (Hodograph.empty())
				THROW_MESSAGE("Hodograph is empty!");

			m_pChart->AddData(RGB(1, 1, 1), Hodograph);
		}

	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}
void CCypressGUIDlg::OnChartClear()
{
	SCOPED_LOG(m_Log);
	m_pChart->ClearData();
}

void CCypressGUIDlg::OnChartStop()
{
	SCOPED_LOG(m_Log);
	m_pChart->Stop();
}

void CCypressGUIDlg::OnChartStart()
{
	SCOPED_LOG(m_Log);
	m_pChart->Start();
}

void CCypressGUIDlg::OnChartHodohraph()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		MessageBox(_T("Specify the calibration data xml file, please."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		_tstring sCalibrationFile;
		{
			LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};
			CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				sCalibrationFile.assign(FileDlg.GetNextPathName(fileNamesPosition));
			}
			else
				return;
		}

		_tstring sMeasurementFile;
		MessageBox(_T("Specify the measurement data xml file, please."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		{
			LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};
			CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				sMeasurementFile.assign(FileDlg.GetNextPathName(fileNamesPosition));
			}
			else
				return;
		}

		m_Hodograph = m_Analyzer.CalculateHodograph(sCalibrationFile, sMeasurementFile, m_bIsNegativePartEnabled, m_HodographFrequencies);
		if (MessageBox(_T("Approximate?"), _T("cyAPI info"), MB_YESNO | MB_ICONINFORMATION) == IDYES)
		{
			m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
			m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
			m_Hodograph = m_Analyzer.Approximate(m_Hodograph);
		}

		if (m_bIsChartPlottingFrom00)
			m_Hodograph.push_front(CPoint(0, 0));

		m_pChart->AddData(RGB(1, 1, 1), m_Hodograph);
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::OnChartPlotfile()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

		CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
		if(FileDlg.DoModal() == IDOK)
		{
			POSITION fileNamesPosition = FileDlg.GetStartPosition();
			const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

			CXmlParser Parser;
			Parser.ParseFile(clrn::CFile::GetCompletePath(sPath));
			CXmlNode Data = Parser.GetData();
			PlotXmlData(Data);
		}
		else
			return;
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}



void CCypressGUIDlg::OnLogShow()
{
	SCOPED_LOG(m_Log);
	m_Log.ShowWindow();
}

void CCypressGUIDlg::OnLogHide()
{
	SCOPED_LOG(m_Log);
	m_Log.HideWindow();
}

void CCypressGUIDlg::OnDataMerge()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

		CFileDialog FileDlg(
			TRUE, 
			_T(".xml"), 
			NULL, 
			OFN_ALLOWMULTISELECT, 
			strFilter, 
			this
		);
		if(FileDlg.DoModal() == IDOK)
		{
			std::vector< _tstring > vecFiles;
			POSITION fileNamesPosition = FileDlg.GetStartPosition();
			while (fileNamesPosition)
			{
				_tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));
				vecFiles.push_back(sPath);
			}
			
			const CXmlNode Merged = m_Analyzer.MergeXmlData(vecFiles);	
			if (Merged.Childs.empty())
			{
				MessageBox(_T("Nothing to save!"), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
				return;
			}

			LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

			CFileDialog FileDlg(FALSE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

				CXmlParser Parser;

				Parser.SetData(Merged);
				Parser.SaveToFile(clrn::CFile::GetCompletePath(sPath));
			}
			else
				return;

		}
		else
			return;
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::OnLogClear()
{
	SCOPED_LOG(m_Log);
	m_Log.Clear();
}

void CCypressGUIDlg::OnClose()
{
	SCOPED_LOG(m_Log);
	// TODO: Add your message handler code here and/or call default
	m_PaintThread.interrupt();
	m_pDevice->Stop();
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	CDialog::OnClose();
}


void CCypressGUIDlg::OnChartShowhodohraph()
{
	SCOPED_LOG(m_Log);
	CMenu * pMenu = GetMenu();
	if (!m_bIsHodographPloting)
	{
		MessageBox(_T("Specify the calibration data xml file, please."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		_tstring sCalibrationFile;
		{
			LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};
			CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				m_sCalibrationFilePath.assign(FileDlg.GetNextPathName(fileNamesPosition));
			}
			else
				return;
		}
		pMenu->CheckMenuItem(ID_CHART_SHOWHODOHRAPH, MF_CHECKED);
		m_bIsHodographPloting = true;
	}
	else
	{
		pMenu->CheckMenuItem(ID_CHART_APPROXIMATEHODOHRAPH, MF_UNCHECKED);
		m_bIsHodographApproximating = false;
		pMenu->CheckMenuItem(ID_CHART_SHOWHODOHRAPH, MF_UNCHECKED);
		m_bIsHodographPloting = false;
	}
}

void CCypressGUIDlg::OnChartShowphase()
{
	SCOPED_LOG(m_Log);
	CMenu * pMenu = GetMenu();
	if (!m_bIsPhasePloting)
	{
		pMenu->CheckMenuItem(ID_CHART_SHOWPHASE, MF_CHECKED);
		m_bIsPhasePloting = true;
	}
	else
	{
		pMenu->CheckMenuItem(ID_CHART_SHOWPHASE, MF_UNCHECKED);
		m_bIsPhasePloting = false;
	}
}

void CCypressGUIDlg::OnApplicationSettings()
{
	SCOPED_LOG(m_Log);
	CSettingsDlg SettingsDlg;

	SettingsDlg.m_nACP						= Settings::Instance().GetACPFrequency();
	SettingsDlg.m_nCAP						= Settings::Instance().GetCAPFrequency();
	SettingsDlg.m_nMinPeriod				= Settings::Instance().GetMinimumPeriodSize();
	SettingsDlg.m_nPeriodsPerBuffer			= Settings::Instance().GetPeriodsPerBuffer();
	SettingsDlg.m_nIterationsCoefficient	= Settings::Instance().GetIterationCoefficient();
	SettingsDlg.m_nMinIterations			= Settings::Instance().GetMinIterations();
	SettingsDlg.m_nMaxIterations			= Settings::Instance().GetMaxIterations();
	SettingsDlg.m_nFindZeroCoefficient		= Settings::Instance().GetFindZeroCoefficient();
	SettingsDlg.m_nApproximationPoints		= Settings::Instance().GetApproximatePoints();
	SettingsDlg.m_nRShunt					= Settings::Instance().GetRShunt();
	SettingsDlg.m_sCalibration				= Settings::Instance().GetCalibrationFileName().c_str();
	SettingsDlg.m_sMeasurement				= Settings::Instance().GetMeasurementFileName().c_str();
	SettingsDlg.m_sAmplitudes				= Settings::Instance().GetAmplitudesFileName().c_str();
	SettingsDlg.m_sCoefficients				= Settings::Instance().GetCoefficientsFileName().c_str();
	SettingsDlg.m_sLogFile					= Settings::Instance().GetLoggerFileName().c_str();
	SettingsDlg.m_nLogLvl					= static_cast< int > (Settings::Instance().GetLogLevel());

	int nLastLogLevel = SettingsDlg.m_nLogLvl;
	
	if (SettingsDlg.DoModal() == IDOK)
	{
		Settings::Instance().SetACPFrequency(SettingsDlg.m_nACP);
		Settings::Instance().SetCAPFrequency(SettingsDlg.m_nCAP);
		Settings::Instance().SetMinimumPeriodSize(SettingsDlg.m_nMinPeriod);
		Settings::Instance().SetPeriodsPerBuffer(SettingsDlg.m_nPeriodsPerBuffer);
		Settings::Instance().SetIterationCoefficient(SettingsDlg.m_nIterationsCoefficient);
		Settings::Instance().SetMinIterations(SettingsDlg.m_nMinIterations);
		Settings::Instance().SetMaxIterations(SettingsDlg.m_nMaxIterations);
		Settings::Instance().SetFindZeroCoefficient(SettingsDlg.m_nFindZeroCoefficient);
		Settings::Instance().SetApproximatePoints(SettingsDlg.m_nApproximationPoints);
		Settings::Instance().SetRShunt(SettingsDlg.m_nRShunt);
		Settings::Instance().SetCalibrationFileName(SettingsDlg.m_sCalibration.GetBuffer());
		Settings::Instance().SetMeasurementFileName(SettingsDlg.m_sMeasurement.GetBuffer());
		Settings::Instance().SetAmplitudesFileName(SettingsDlg.m_sAmplitudes.GetBuffer());
		Settings::Instance().SetCoefficientsFileName(SettingsDlg.m_sCoefficients.GetBuffer());
		Settings::Instance().SetLoggerFileName(SettingsDlg.m_sLogFile.GetBuffer());
		Settings::Instance().SetLogLevel(static_cast< CLog::Level::Enum_t > (SettingsDlg.m_nLogLvl));	

		Settings::Instance().Save();

		if (nLastLogLevel != SettingsDlg.m_nLogLvl)
			m_Log.SetLevel(static_cast< CLog::Level::Enum_t > (SettingsDlg.m_nLogLvl));
	}
}

void CCypressGUIDlg::OnDataClear()
{
	SCOPED_LOG(m_Log);
	boost::mutex::scoped_lock Lock(m_mxChartData);
	m_AmplitudeIn.clear();
	m_PhaseIn.clear();
	m_AmplitudeOut.clear();
	m_PhaseOut.clear();
	m_pDevice->ClearData();
}


void CCypressGUIDlg::OnChartSavehodograph()
{
	SCOPED_LOG(m_Log);
	TRY_EXCEPTIONS
	{
		if (m_sCalibrationFilePath.empty())
		{
			MessageBox(_T("Specify the calibration data xml file, please."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
			_tstring sCalibrationFile;
			{
				LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};
				CFileDialog FileDlg(TRUE, _T(".xml"), NULL, 0, strFilter, this);
				if(FileDlg.DoModal() == IDOK)
				{
					POSITION fileNamesPosition = FileDlg.GetStartPosition();
					m_sCalibrationFilePath.assign(FileDlg.GetNextPathName(fileNamesPosition));
				}
				else
					return;
			}
		}
		{
			if (m_xmlCalibrationData.Childs.empty())
			{
				CXmlParser Parser;
				Parser.ParseFile(clrn::CFile::GetCompletePath(m_sCalibrationFilePath));
				m_xmlCalibrationData = Parser.GetData();
			}

			CXmlNode xmlData = m_pDevice->GetData();

			CChart::TData Hodograph = m_Analyzer.CalculateHodograph(m_xmlCalibrationData, xmlData, m_bIsNegativePartEnabled);

			CXmlNode xmlHodograph;
			xmlHodograph.Data[TAG_NAME] = _T("Hodograph");

			CChart::TData::const_iterator it = Hodograph.begin();
			const CChart::TData::const_iterator itEnd = Hodograph.end();
			for (; it != itEnd; ++it)
			{
				CXmlNode xmlNode;
				xmlNode.Data[TAG_NAME] = _T("point");
				xmlNode.Data[_T("re")] = boost::lexical_cast< _tstring > (it->x);
				xmlNode.Data[_T("im")] = boost::lexical_cast< _tstring > (it->y);
			
				xmlHodograph.Childs.push_back(xmlNode);
			}

			LPCTSTR strFilter = {_T("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||")};

			CFileDialog FileDlg(FALSE, _T(".xml"), NULL, 0, strFilter, this);
			if(FileDlg.DoModal() == IDOK)
			{
				POSITION fileNamesPosition = FileDlg.GetStartPosition();
				const _tstring sPath(FileDlg.GetNextPathName(fileNamesPosition));

				CXmlParser Parser;

				Parser.SetData(xmlHodograph);
				Parser.SaveToFile(clrn::CFile::GetCompletePath(sPath));
			}
			else
				return;
		}
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(m_Log);
}

void CCypressGUIDlg::OnChartApproximatehodohraph()
{
	SCOPED_LOG(m_Log);
	CMenu * pMenu = GetMenu();
	if (!m_bIsHodographPloting)
	{
		pMenu->CheckMenuItem(ID_CHART_APPROXIMATEHODOHRAPH, MF_UNCHECKED);
		m_bIsHodographApproximating = false;
		MessageBox(_T("You must enable \"Show hodograph\" first."), _T("cyAPI info"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	if (!m_bIsHodographApproximating)
	{
		pMenu->CheckMenuItem(ID_CHART_APPROXIMATEHODOHRAPH, MF_CHECKED);
		m_bIsHodographApproximating = true;
	}
	else if (m_bIsHodographApproximating)
	{
		pMenu->CheckMenuItem(ID_CHART_APPROXIMATEHODOHRAPH, MF_UNCHECKED);
		m_bIsHodographApproximating = false;
	}	
}

void CCypressGUIDlg::OnChartStartchartfrom00()
{
	SCOPED_LOG(m_Log);
	CMenu * pMenu = GetMenu();
	if (!m_bIsChartPlottingFrom00)
	{
		pMenu->CheckMenuItem(ID_CHART_STARTCHARTFROM, MF_CHECKED);
		m_bIsChartPlottingFrom00 = true;
	}
	else
	{
		pMenu->CheckMenuItem(ID_CHART_STARTCHARTFROM, MF_UNCHECKED);
		m_bIsChartPlottingFrom00 = false;
	}
}

void CCypressGUIDlg::OnChartShownegativepart()
{
	SCOPED_LOG(m_Log);
	CMenu * pMenu = GetMenu();
	if (!m_bIsNegativePartEnabled)
	{
		pMenu->CheckMenuItem(ID_CHART_SHOWNEGATIVEPART, MF_CHECKED);
		m_bIsNegativePartEnabled = true;
	}
	else
	{
		pMenu->CheckMenuItem(ID_CHART_SHOWNEGATIVEPART, MF_UNCHECKED);
		m_bIsNegativePartEnabled = false;
	}
}
