#pragma once


#include "stdafx.h"
#include "resource.h"

#include <boost/function.hpp>
// CLogWnd dialog


class CLogWnd : public CDialog
{
	DECLARE_DYNAMIC(CLogWnd)

public:
	//! ��� �-��� ��� ������ ����������� ��������� ���� �������
	typedef boost::function< void (void) > TResizeFun;

	CLogWnd(TResizeFun& ResizeFun, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogWnd();

// Dialog Data
	enum { IDD = IDD_LOGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//! ������� ��������� ��������� ������� ����
	TResizeFun m_ResizeFun;
};
