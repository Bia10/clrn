#pragma once

#include "Exception.h"
// CTwoControl dialog

class CTwoControl : public CDialog
{
	DECLARE_DYNAMIC(CTwoControl)

public:
	CTwoControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTwoControl();

// Dialog Data
	enum { IDD = IDD_DIALOGWITHTWO };

protected:
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// ��������� ����� 
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nSlider1;

	int m_nSlider2;

	int m_nStep;
	afx_msg void OnNMCustomdrawSliderf1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderf2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusIndicatorF1();
	afx_msg void OnEnKillfocusIndicatorF2();
	afx_msg void OnNMReleasedcaptureSliderf2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderf1(NMHDR *pNMHDR, LRESULT *pResult);


	afx_msg void OnEnChangeStep();

	//! �������� ������ �������
	const int GetLowerFrequency() const;

	//! �������� ������� �������
	const int GetUpperFrequency() const;

	//! �������� ���
	const int GetStep() const;

private:

	//! ����� ��� ��������� ����
	CBrush m_BackGroundBrush;	
};
