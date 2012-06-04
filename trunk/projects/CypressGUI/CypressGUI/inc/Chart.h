#pragma once

// Include Project headers
#include "stdafx.h"

// Include STL headers
#include <string>
#include <list>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>

// Include Boost headers
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

//! Объект работы с графиками
class CChart
{
public:
	CChart();
	CChart(CWnd * pWindow, const int nX, const int nY, const int nSizeX, const int nSizeY, const _tstring sXname, const _tstring sYname);
	~CChart(void);

	//! Тип данных графика
	typedef std::list< CPoint > TData;

	//! Тип контейнера со всеми данными графика
	typedef std::map< COLORREF, TData > TAllData;

	//! Запустить поток отрисовки
	void Start();

	//! Остановить поток отрисовки
	void Stop();

	//! Установить цвет фона
	void SetBackGroundColor(const COLORREF dwColor);

	//! Очистить даныне
	void ClearData();

	//! Получить цвет графика ближе всего к которому мышь
	const COLORREF GetActiveChartColor() const;

	//! Получить значение точки ближе всего к которому мышь
	const CPoint GetActivePoint() const;

	//! Добавить данные для отрисовки
	void AddData(const COLORREF dwColor, const TData& Data);

	//! Добавить данные для отрисовки
	void AddData(const COLORREF dwColor, const std::list< int >& Data);

	//! Добавить данные для отрисовки
	void AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize);

	//! Добавить данные для отрисовки
	void AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize, bool Parity);

private:
	//! Отрисовать границы
	void PaintBorder();

	//! Отрисовать значение графика под машью
	void PaintValue();

	//! Инициализировать тултип
	void InitToolTip();

	//! Отрисовать значения графика
	void PaintData();

	//! Потоковая ф-ция отрисовки данных
	void PaintThread();

	//! Попадает ли курсор мыши в область графика
	bool IsMouseInChartArea();

	//! Отрисовать тултип в указанной области
	void PaintTooltip(const CPoint& Point, const _tstring& sText);

	//! Контекст окна
	CClientDC m_DC;

	//! Хэндл окна
	CWnd * m_hWindow;

	//! handle to the ToolTip control	
	HWND m_hWndToolTip;     

	//! ToolTip info
	TOOLINFO m_ToolTipInfo;

	//! Начальные координаты графика
	const int m_nCornerX;
	const int m_nCornerY;

	//! Размер по Х
	const int m_nSizeX;

	//! Размер по Y
	const int m_nSizeY;

	//! Размер клетки по Х
	static const int CELL_SIZE_X;

	//! Размер клетки по Y
	static const int CELL_SIZE_Y;

	//! Максимальное кол-во принимаемых данных
	static const int MAX_DATA_SIZE;

	//! Имя оси по Х
	const _tstring m_sXname;

	//! Имя оси по Y
	const _tstring m_sYname;

	//! Показывается ли тултип в данный момент
	bool m_bToolTipActivated;

	//! Данные графика
	TAllData m_Data;

	//! Поток отрисовки
	boost::thread m_threadWork;

	//! Мьютекс на данные графика
	boost::mutex m_mxData;

	//! Флаг работы потока отрисовки
	bool m_bIsWorking;

	//! Цвет фона графика
	COLORREF m_dwBackGroundColor;

	//! Цвет графика ближе всего к которому мышь
	COLORREF m_dwActiveChartColor;

	//! Текущее значение точки ближе всего к которому мышь
	CPoint m_pointActivePoint;
};

