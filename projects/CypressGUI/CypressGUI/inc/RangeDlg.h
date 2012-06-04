#pragma once
#include "afxwin.h"

//! Include STL library headers
#include <vector>

//! Include project headers
#include "stdafx.h"
#include "TwoControl.h"
#include "OneControl.h"

//! Include boost library headers
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>


// CRangeDlg dialog

struct CRange
{
	int	Lower;
	int Upper;
	int Step;
};

class CRangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CRangeDlg)

public:
	//! ��� ������� ���������� ������
	typedef boost::function< void(void) > TUpdateFun;

	//! ��� ������� ��������
	typedef boost::function< void(const std::vector< CRange >&) > TCallBackFun;

	CRangeDlg(
		int& nLowerFrequency,
		int& nUpperFrequency,
		int& nStep,
		TUpdateFun& UpdateFun, 
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CRangeDlg();

// Dialog Data
	enum { IDD = IDD_RANGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	//! ���������������� ������ �������
	void Init(TCallBackFun& CallBackFun);
	
private:

	//! �������� ��������
	void Add();

	//! ������� ��������
	void Remove();

	//! ������� ������ ����������
	CListBox					m_RangesCtrl;

	//! ������ �� ������ �������
	int&						m_nLowerFrequency;

	//! ������ �� ������� �������
	int&						m_nUpperFrequency;

	//! ������ �� ���
	int&						m_nStep;

	//! ������� ����������
	TUpdateFun					m_funUpdate;

	//! ������� ��������
	TCallBackFun				m_funCallBack;

public:
	afx_msg void OnBnClickedDefault();
};
