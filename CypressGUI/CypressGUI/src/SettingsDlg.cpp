// src/SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "inc/SettingsDlg.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, m_nACP(0)
	, m_nCAP(0)
	, m_nMinPeriod(0)
	, m_nPeriodsPerBuffer(0)
	, m_nIterationsCoefficient(0)
	, m_nMinIterations(0)
	, m_nMaxIterations(0)
	, m_sCalibration(_T(""))
	, m_sMeasurement(_T(""))
	, m_sAmplitudes(_T(""))
	, m_sCoefficients(_T(""))
	, m_sLogFile(_T(""))
	, m_nLogLvl(0)
	, m_bIsOK(false)
	, m_nFindZeroCoefficient(0)
	, m_nApproximationPoints(0)
	, m_nRShunt(0)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nACP);
	DDX_Text(pDX, IDC_EDIT2, m_nCAP);
	DDX_Text(pDX, IDC_EDIT3, m_nMinPeriod);
	DDV_MinMaxInt(pDX, m_nMinPeriod, 1, 1000000);
	DDX_Text(pDX, IDC_EDIT4, m_nPeriodsPerBuffer);
	DDV_MinMaxInt(pDX, m_nPeriodsPerBuffer, 1, 100);
	DDX_Text(pDX, IDC_EDIT5, m_nIterationsCoefficient);
	DDV_MinMaxInt(pDX, m_nIterationsCoefficient, 1, 100000);
	DDX_Text(pDX, IDC_EDIT6, m_nMinIterations);
	DDV_MinMaxInt(pDX, m_nMinIterations, 1, 100000);
	DDX_Text(pDX, IDC_EDIT7, m_nMaxIterations);
	DDV_MinMaxInt(pDX, m_nMaxIterations, 1, 10000);
	DDX_Text(pDX, IDC_EDIT8, m_sCalibration);
	DDX_Text(pDX, IDC_EDIT9, m_sMeasurement);
	DDX_Text(pDX, IDC_EDIT10, m_sAmplitudes);
	DDX_Text(pDX, IDC_EDIT11, m_sCoefficients);
	DDX_Text(pDX, IDC_EDIT12, m_sLogFile);
	DDX_CBIndex(pDX, IDC_COMBO1, m_nLogLvl);
	DDX_Text(pDX, IDC_EDIT13, m_nFindZeroCoefficient);
	DDV_MinMaxInt(pDX, m_nFindZeroCoefficient, 1, 10000000);
	DDX_Text(pDX, IDC_EDIT14, m_nApproximationPoints);
	DDV_MinMaxInt(pDX, m_nApproximationPoints, 1, 1000);
	DDX_Text(pDX, IDC_EDIT15, m_nRShunt);
	DDV_MinMaxInt(pDX, m_nRShunt, 1, 100000000);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingsDlg message handlers

void CSettingsDlg::OnBnClickedOk()
{
	m_bIsOK = true;
	// TODO: Add your control notification handler code here
	OnOK();
}
