// PerceptronDlg.cpp : implementation file
//

//! Include project headers
#include "stdafx.h"
#include "Perceptron.h"
#include "PerceptronDlg.h"
#include "TeachDlg.h"

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


// CPerceptronDlg dialog




CPerceptronDlg::CPerceptronDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPerceptronDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPerceptronDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_FileList);
	DDX_Control(pDX, IDC_LOG_LIST, m_LogListCtrl);
}

BEGIN_MESSAGE_MAP(CPerceptronDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CPerceptronDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ITEMADD, &CPerceptronDlg::OnBnClickedAddbutton)
	ON_BN_CLICKED(IDC_ITEMREMOVE, &CPerceptronDlg::OnBnClickedItemremove)
	ON_BN_CLICKED(IDC_TEACH, &CPerceptronDlg::OnBnClickedTeach)
	ON_BN_CLICKED(IDC_TEST, &CPerceptronDlg::OnBnClickedTest)
END_MESSAGE_MAP()


// CPerceptronDlg message handlers

BOOL CPerceptronDlg::OnInitDialog()
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
	
	WriteLog(L"Perceptron successfully started.");

	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPerceptronDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPerceptronDlg::OnPaint()
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
HCURSOR CPerceptronDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPerceptronDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	this->UpdateData();
	// TODO: Add your control notification handler code here
	OnOK();
}

void CPerceptronDlg::OnBnClickedAddbutton()
{
	try
	{
		CTeachDlg TeachDlg(this);
		TeachDlg.DoModal();

		const _tstring sFilePath = TeachDlg.GetPath();
		const bool bType = TeachDlg.GetType();

		if (sFilePath.empty())
			return;

		const _tstring sItem((bType ? _tstring(L"Type: European.") : _tstring(L"Type: Another.")) + _tstring(L" Path: ") + sFilePath.c_str());
		const int nIndex = m_FileList.FindStringExact(0, sItem.c_str());
		if (nIndex == -1)
		{
			m_FileList.AddString(sItem.c_str());
			WriteLog(L"Added item: " + sItem);
			int nCount = m_FileList.GetCount();
			m_FileList.SetCurSel(nCount - 1);
		}
		else
			MessageBox(L"This image already exists in the teach list!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
	}
	catch(std::exception& e)
	{
		WriteLog(e.what());
	}
	catch(...)
	{
		WriteLog("Bad news, unhandled exception occured :(");
	}

}

void CPerceptronDlg::OnBnClickedItemremove()
{
	try
	{
		const int nCurrent = m_FileList.GetCurSel();
		if (nCurrent == -1)
		{
			MessageBox(L"Select item first!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		CString sTemp;
		m_FileList.GetText(nCurrent, sTemp);
		m_FileList.DeleteString(nCurrent);
		WriteLog(_tstring(L"Deleted item: ") + sTemp.GetBuffer());
	}
	catch(std::exception& e)
	{
		WriteLog(e.what());
	}
	catch(...)
	{
		WriteLog("Bad news, unhandled exception occured :(");
	}
}

void CPerceptronDlg::OnBnClickedTeach()
{	
	try
	{
		WriteLog(L"Started teaching...");
		int nCount = m_FileList.GetCount();
		if (!nCount)
		{
			MessageBox(L"Image to teach does not exists! Please add some images.", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
			WriteLog(L"Teaching failed!");
			return;
		}
		if (m_NeuroNetwork.IsNetworkteached())
		{
			MessageBox(L"Neuro network already teached!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
			WriteLog(L"Teaching failed!");
			return;
		}

		m_TeachList.clear();
		m_Images.clear();

		for (int i = 0; i < nCount; ++i)
		{
			CString sTemp;
			m_FileList.GetText(i, sTemp);
			const _tstring sItem(sTemp.GetBuffer());
			m_TeachList.push_back(sItem);
		}

		// Формируем карту с id результата и именем изображения
		bool bId = 0;

		std::vector< _tstring >::const_iterator it = m_TeachList.begin();
		const std::vector< _tstring >::const_iterator itEnd = m_TeachList.end();
		for (; it != itEnd; ++it)
		{
			const _tstring sCurrent = *it;
			_tstring::size_type nPath = sCurrent.rfind(L"Path: ");
			_tstring::size_type nEuro = sCurrent.find(L"Type: ");
			if (nPath != _tstring::npos && nEuro != _tstring::npos)
			{
				const _tstring sEuro(&sCurrent.c_str()[nEuro + 6], &sCurrent.c_str()[nPath]);
				const bool bEuro = sEuro == _T("European. ");
				const _tstring sPath(&sCurrent.c_str()[nPath + 6]);
				m_Images.push_back(std::pair < _tstring, bool > (sPath, bEuro));
			}
		}

		// Инициализируем нейронную сеть
		// Кол-во нейронов - 100
		// Кол-во результатов - 1
		m_NeuroNetwork.Init(100, 1, 200000);

		// Обучаем нейронную сеть
		const _tstring sTeachResult = m_NeuroNetwork.Teach(m_Images);

		WriteLog(_tstring(L"Teaching completed! Result : ") + sTeachResult);
	}
	catch(std::exception& e)
	{
		WriteLog(e.what());
	}
	catch(...)
	{
		WriteLog("Bad news, unhandled exception occured :(");
	}
}

void CPerceptronDlg::WriteLog(const std::string& sData)
{
	try
	{ 
		CTime Current;
		const _tstring sTime(Current.GetCurrentTime().Format("[%H:%M:%S] "));
		const _tstring sText(sTime + vr::str::widen(sData, std::locale()));
		m_LogListCtrl.AddString(sText.c_str());

		int nCount = m_LogListCtrl.GetCount();
		m_LogListCtrl.SetCurSel(nCount - 1);
	}
	catch(std::exception& e)
	{
		MessageBoxA(0, e.what(), "Perceptron error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		MessageBoxA(0, "Unexpected error!", "Perceptron error", MB_OK | MB_ICONERROR);
	}

}


void CPerceptronDlg::WriteLog(const std::wstring& sData)
{
	try
	{
		CTime Current;
		const _tstring sTime(Current.GetCurrentTime().Format("[%H:%M:%S] "));
		const _tstring sText(sTime + sData);
		m_LogListCtrl.AddString(sText.c_str());

		int nCount = m_LogListCtrl.GetCount();
		m_LogListCtrl.SetCurSel(nCount - 1);

		Sleep(50);
	}
	catch(std::exception& e)
	{
		MessageBoxA(0, e.what(), "Perceptron error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		MessageBoxA(0, "Unexpected error!", "Perceptron error", MB_OK | MB_ICONERROR);
	}

}


void CPerceptronDlg::OnBnClickedTest()
{
	try
	{
		WriteLog("Started recognizing...");
		Sleep(50);

		if (!m_NeuroNetwork.IsNetworkteached())
		{
			MessageBox(L"You must teach network first!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
			WriteLog("Recognizing failed!");
			return;
		}

		// Получаем имя файла для распознавания
		LPCTSTR strFilter = { L"BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

		CFileDialog FileDlg(FALSE, L".bmp", NULL, 0, strFilter, this);
		_tstring sPath;

		if( FileDlg.DoModal() == IDOK )
			sPath.assign(_tstring(FileDlg.GetFolderPath()) + L"\\" + _tstring(FileDlg.GetFileName()));
		else
		{
			WriteLog("Recognizing failed, image was not selected!");
			return;
		}

		// Распознаем
		const float nResult = m_NeuroNetwork.Recognize(sPath);
		const _tstring sResult = boost::lexical_cast< _tstring > (nResult);

		_tstring sDelta = _T("\nDelta = ");
		float fDelta = nResult;
		_tstring sMessage = _T("Money on this picture was");
		if (nResult < 0.5)
		{	
			sDelta += boost::lexical_cast< _tstring > (fDelta);
			sMessage += _T("n't european!");
			MessageBox(_tstring(sMessage + sDelta).c_str(), _T("Perceptron result"), MB_OK | MB_ICONINFORMATION);

		}
		else 
		{
			fDelta = 1 - nResult;
			sDelta += boost::lexical_cast< _tstring > (fDelta);
			sMessage += _T(" a european!");
			MessageBox(_tstring(sMessage + sDelta).c_str(), _T("Perceptron result"), MB_OK | MB_ICONINFORMATION);
		}
		_tstring sLogMessage  = _T("Recognize result = ");
		sLogMessage += sResult;
		WriteLog(sLogMessage.c_str());
		WriteLog(sDelta.c_str());
		WriteLog(sMessage);
		WriteLog("Recognizing finished!");
	}
	catch(std::exception& e)
	{
		MessageBoxA(0, e.what(), "Perceptron error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		MessageBoxA(0, "Unexpected error!", "Perceptron error", MB_OK | MB_ICONERROR);
	}
}
