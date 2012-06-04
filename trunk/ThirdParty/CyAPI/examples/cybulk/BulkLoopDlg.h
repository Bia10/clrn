// BulkLoopDlg.h : header file
//

#if !defined(AFX_BULKLOOPDLG_H__D3E75ECD_0ADD_4838_B4EE_E39E6FC7B4B8__INCLUDED_)
#define AFX_BULKLOOPDLG_H__D3E75ECD_0ADD_4838_B4EE_E39E6FC7B4B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CyAPI.h"

/////////////////////////////////////////////////////////////////////////////
// CBulkLoopDlg dialog

class CBulkLoopDlg : public CDialog
{
// Construction
public:
	CBulkLoopDlg(CWnd* pParent = NULL);	// standard constructor
	CCyUSBDevice *USBDevice;

	CCyUSBEndPoint *OutEndpt;
	CCyUSBEndPoint *InEndpt;

    int DeviceIndex;

	CWinThread *XferThread;

	int DataFillMethod;
	bool bLooping;

	//bool RegForUSBEvents(void);


// Dialog Data
	//{{AFX_DATA(CBulkLoopDlg)
	enum { IDD = IDD_BULKLOOP_DIALOG };
	CButton	m_DisableTimeoutChkBox;
	CButton	m_TestBtn;
	CButton	m_RefreshBtn;
	CComboBox	m_DeviceListComBox;
	CButton	m_StartBtn;
	CStatic	m_StatusLabel;
	CComboBox	m_FillPatternComBox;
	CButton	m_StopOnErrorChkBox;
	CEdit	m_SeedValue;
	CStatic	m_FailureCount;
	CStatic	m_SuccessCount;
	CEdit	m_XferSize;
	CComboBox	m_InEndptComBox;
	CComboBox	m_OutEndptComBox;
	int		m_DataValueRadioBtns;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBulkLoopDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBulkLoopDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartBtn();
	afx_msg void OnSelchangeOutCombox();
	afx_msg void OnSelchangeInCombox();
	afx_msg void OnResetBtn();
	afx_msg void OnRefreshBtn();
	afx_msg void OnTestBtn();
	afx_msg void OnSelchangeDeviceListCombox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BULKLOOPDLG_H__D3E75ECD_0ADD_4838_B4EE_E39E6FC7B4B8__INCLUDED_)
