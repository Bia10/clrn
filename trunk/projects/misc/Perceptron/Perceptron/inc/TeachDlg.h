#pragma once
#include "afxwin.h"

#include "stdafx.h"

// CTeachDlg dialog

class CTeachDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachDlg)

public:
	CTeachDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTeachDlg();

	virtual void OnFinalRelease();

// Dialog Data
	enum { IDD = IDD_TEACHDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedOkteach();
	afx_msg void OnBnClickedCancelteach();

	//! �������� ���� � �����
	const _tstring GetPath() const;

	//! �������� ��� �����������
	const bool GetType() const;

private:

	//! ������� ��������� ��� ���� �����������
	CComboBox m_ComboTypeCtrl;

	// ! ������� ��������� ��� ���� � �����
	CEdit m_FilePath;

	//! ���� � �����
	_tstring m_sPath;

	//! ��� �����������
	bool m_bType;

};
