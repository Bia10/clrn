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

	//! Получить путь к файлу
	const _tstring GetPath() const;

	//! Получить тип изображения
	const bool GetType() const;

private:

	//! Контрол листбокса для типа изображения
	CComboBox m_ComboTypeCtrl;

	// ! Контрол едитбокса для пути к файлу
	CEdit m_FilePath;

	//! Путь к файлу
	_tstring m_sPath;

	//! Тип изображения
	bool m_bType;

};
