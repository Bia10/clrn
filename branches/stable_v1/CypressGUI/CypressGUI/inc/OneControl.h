#pragma once

#include "Exception.h"

// COneControl dialog

class COneControl : public CDialog
{
	DECLARE_DYNAMIC(COneControl)

public:
	COneControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~COneControl();

// Dialog Data
	enum { IDD = IDD_ONECONTROL };

protected:
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// ��������� ����� 
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int m_nSlider;
	afx_msg void OnNMCustomdrawSliderf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusIndicatorF();

	//! �������� �������
	const int GetFrequency() const;

private:

	//! ����� ��� ��������� ����
	CBrush m_BackGroundBrush;	
};
