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
	//! Тип функции обновления данных
	typedef boost::function< void(void) > TUpdateFun;

	//! Тип функции коллбэка
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

	//! Инициализировать данные диалога
	void Init(TCallBackFun& CallBackFun);
	
private:

	//! Добавить диапазон
	void Add();

	//! Удалить диапазон
	void Remove();

	//! Контрол списка диапазонов
	CListBox					m_RangesCtrl;

	//! Ссылка на нижнюю частоту
	int&						m_nLowerFrequency;

	//! Ссылка на верхнюю частоту
	int&						m_nUpperFrequency;

	//! Ссылка на шаг
	int&						m_nStep;

	//! Функция обновления
	TUpdateFun					m_funUpdate;

	//! Функция коллбэка
	TCallBackFun				m_funCallBack;

public:
	afx_msg void OnBnClickedDefault();
};
