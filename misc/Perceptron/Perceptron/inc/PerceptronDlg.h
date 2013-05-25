// PerceptronDlg.h : header file
//

#pragma once
//! MFC graphic headers
#include "afxwin.h"
#include "afxcmn.h"

//! Include project headers
#include "NeuroNetwok.h"
#include "Conversion.h"

//! Include STl library headers
#include <map>

//! Include boost library headers
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

// CPerceptronDlg dialog
class CPerceptronDlg : public CDialog
{
// Construction
public:
	CPerceptronDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PERCEPTRON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
	afx_msg void OnTcnSelchangeLogtab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingLogtab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAddbutton();
	afx_msg void OnBnClickedItemremove();
	afx_msg void OnBnClickedTeach();
	afx_msg void OnBnClickedTest();
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	//! Добавить запись в лог общих событий
	void												WriteLog(const std::string& sData);

	//! Добавить запись в лог общих событий
	void												WriteLog(const std::wstring& sData);

	//! Контрол списка изображений для обучения
	CListBox											m_FileList;

	//! Список изображений для обучения
	std::vector< _tstring >								m_TeachList;

	//! Таб-контрол логгера
	CTabCtrl											m_LogTabCtrl;

	//! Контрол листбокса логгера
	CListBox											m_LogListCtrl;

	//! Контейнер имен изображений и их признаку(европейская валюта или нет)
	std::vector< std::pair < _tstring, bool > >			m_Images;

	//! Нейронная сеть
	CNeuroNetwok										m_NeuroNetwork;

};
