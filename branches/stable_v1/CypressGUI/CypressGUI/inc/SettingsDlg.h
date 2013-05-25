#pragma once

#include "Settings.h"

// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	long m_nACP;
	long m_nCAP;
	int m_nMinPeriod;
	int m_nPeriodsPerBuffer;
	int m_nIterationsCoefficient;
	int m_nMinIterations;
	int m_nMaxIterations;
	CString m_sCalibration;
	CString m_sMeasurement;
	CString m_sAmplitudes;
	CString m_sCoefficients;
	CString m_sLogFile;
	int m_nLogLvl;
	bool m_bIsOK;
	afx_msg void OnBnClickedOk();
	int m_nFindZeroCoefficient;
	int m_nApproximationPoints;
	int m_nRShunt;
};
