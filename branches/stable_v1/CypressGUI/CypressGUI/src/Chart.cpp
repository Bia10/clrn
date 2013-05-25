#include "StdAfx.h"
#include "Chart.h"

const int CChart::CELL_SIZE_X = 10;
const int CChart::CELL_SIZE_Y = 10;
const int CChart::MAX_DATA_SIZE = 500;

CChart::CChart() : 
	m_DC(0),
	m_nCornerX(0),
	m_nCornerY(0),
	m_nSizeX(0),
	m_nSizeY(0)
{

}

CChart::CChart(CWnd * pWindow, const int nX, const int nY, const int nSizeX, const int nSizeY, const _tstring sXname, const _tstring sYname) :
	m_DC(pWindow),
	m_hWindow(pWindow),
	m_nCornerX(nX),
	m_nCornerY(nY),
	m_nSizeX(nSizeX),
	m_nSizeY(nSizeY),
	m_sXname(sXname),
	m_sYname(sYname),
	m_bToolTipActivated(false),
	m_bIsWorking(false),
	m_dwBackGroundColor(RGB(0, 0, 0))
{

}

CChart::~CChart(void)
{
}

void CChart::InitToolTip()
{
	// CREATE A TOOLTIP WINDOW
	m_hWndToolTip = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		TTS_NOPREFIX | TTS_ALWAYSTIP,		
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL
		);

	CRect rect(0,0,90,30);


	// INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE
	m_ToolTipInfo.cbSize = sizeof(TOOLINFO);
	m_ToolTipInfo.uFlags = TTF_TRACK;
	m_ToolTipInfo.hwnd = NULL;
	m_ToolTipInfo.hinst = NULL;
	m_ToolTipInfo.uId = 0;
	m_ToolTipInfo.lpszText = reinterpret_cast< LPWSTR > ("");		
	// ToolTip control will cover the whole window
	m_ToolTipInfo.rect.left = 0;
	m_ToolTipInfo.rect.top = 0;
	m_ToolTipInfo.rect.right = 0;
	m_ToolTipInfo.rect.bottom = 0;


	// SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW
	::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_ToolTipInfo);

	m_bToolTipActivated = true;
}

void CChart::SetBackGroundColor(const COLORREF dwColor)
{
	m_dwBackGroundColor = dwColor;
}

const COLORREF CChart::GetActiveChartColor() const
{
	return m_dwActiveChartColor;
}

const CPoint CChart::GetActivePoint() const
{
	return m_pointActivePoint;
}

void CChart::Start()
{
	if (!m_bIsWorking)
	{
		m_bIsWorking = true;
		m_threadWork = boost::thread(boost::bind(&CChart::PaintThread, this));
	}
}


void CChart::Stop()
{
	if (m_bIsWorking)
	{
		m_bIsWorking = false;
		m_threadWork.join();
	}
}

void CChart::PaintThread()
{
	while (m_bIsWorking)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(40));
		boost::mutex::scoped_lock Lock(m_mxData);
		PaintBorder();
		PaintData();
	}
}

void CChart::ClearData()
{
	boost::mutex::scoped_lock Lock(m_mxData);
	m_Data.clear();
}

void CChart::AddData(const COLORREF dwColor, const TData& Data)
{
	boost::mutex::scoped_lock Lock(m_mxData);
	m_Data[dwColor] = Data;
}

void CChart::AddData(const COLORREF dwColor, const std::list< int >& Data)
{
	int nCoefficient = 1;
	if (Data.size() > MAX_DATA_SIZE)
		nCoefficient = Data.size() / MAX_DATA_SIZE;

	boost::mutex::scoped_lock Lock(m_mxData);
	m_Data[dwColor].clear();
	std::list< int >::const_iterator it = Data.begin();
	const std::list< int >::const_iterator itEnd = Data.end();
	int nCount = 0;
	for (; it != itEnd; ++it)
	{
		bool bPoint = nCount % nCoefficient;
		if (!bPoint)
			m_Data[dwColor].push_back(CPoint(nCount / nCoefficient, *it));
		++nCount;
	}
}

void CChart::AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize)
{
	int nCoefficient = 1;
	if (nSize > MAX_DATA_SIZE)
		nCoefficient = nSize / MAX_DATA_SIZE;

	int nCount = 0;
	boost::mutex::scoped_lock Lock(m_mxData);
	for (unsigned int i = 0 ; i < nSize; i += nCoefficient)
		m_Data[dwColor].push_back(CPoint(nCount++, static_cast< int > (pBuffer[i])));
}

void CChart::AddData(const COLORREF dwColor, const char * pBuffer, const unsigned int nSize, bool Parity)
{
	boost::mutex::scoped_lock Lock(m_mxData);
	for (unsigned int i = 0 ; i < nSize; ++i)
	{
		if (i > MAX_DATA_SIZE * 2)
			break;
		bool bParity = i % 2;
		if (Parity && bParity)
			m_Data[dwColor].push_back(CPoint(i, static_cast< int > (pBuffer[i])));
	}
}

bool CChart::IsMouseInChartArea()
{
	// Координаты мыши
	POINT Mouse;
	GetCursorPos(&Mouse);

	// Координаты окна
	CRect Window;
	m_hWindow->GetWindowRect(Window);

	// Координаты мыши относительно окна
	const int nMouseX = Mouse.x - Window.left;
	const int nMouseY = Mouse.y - Window.top - 40;

	// Определяем попадает ли мышь в область графика
	if (m_nCornerX < nMouseX + 30 && 
		m_nCornerX + m_nSizeX + 30 > nMouseX &&
		m_nCornerY < nMouseY + 30 &&
		m_nCornerY + m_nSizeY + 30 > nMouseY
		)
		return true;
	return false;
}

void CChart::PaintTooltip(const CPoint& Point, const _tstring& sText)
{	
	if (m_Data.empty())
		return;
	
	// Координаты мыши
	POINT Mouse;
	GetCursorPos(&Mouse);

	// Координаты окна
	CRect Window;
	m_hWindow->GetWindowRect(Window);

	// Координаты мыши относительно окна
	const int nMouseX = Mouse.x - Window.left;
	const int nMouseY = Mouse.y - Window.top - 40;

	// Определяем попадает ли мышь в область графика
	if (IsMouseInChartArea())
	{
		if (!m_bToolTipActivated)
			InitToolTip();

		// Линия "прицела"
		CPen PenBorder;
		PenBorder.CreatePen(PS_DOT, 2, RGB(0, 0, 255));
		m_DC.SelectObject(&PenBorder);

		// Отрисовываем "прицел" в точке под мышью
		const int nCrossX = Point.x;
		const int nCrossY = Point.y;

		// Рисуем "прицел" только если он не выходит за пределы графика
		m_DC.MoveTo(nCrossX - 5, nCrossY);
		m_DC.LineTo(nCrossX + 5, nCrossY);

		m_DC.MoveTo(nCrossX, nCrossY - 5);
		m_DC.LineTo(nCrossX, nCrossY + 5);

		m_ToolTipInfo.lpszText = (LPWSTR) sText.c_str();

		// Получаем абсолютные координаты из координат точки на графике
		const int nToolTipX = nCrossX + Window.left;
		const int nToolTipY = nCrossY + Window.top + 20;


		// Ориентируем тултип
		::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(nToolTipX - 10, nToolTipY - 20));
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, true, (LPARAM)(LPTOOLINFO) &m_ToolTipInfo);
		::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, true, (LPARAM)(LPTOOLINFO) &m_ToolTipInfo);

	}
	else
	{
		// Закрываем окно с тултипом
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, false, (LPARAM)(LPTOOLINFO) &m_ToolTipInfo);
		::SendMessage(m_hWndToolTip, WM_CLOSE, false, (LPARAM)(LPTOOLINFO) &m_ToolTipInfo);
		m_bToolTipActivated = false;
	}
}

void CChart::PaintBorder()
{
	// Определяем "0"
	const int nStartX = m_nCornerX;
	const int nStartY = m_nCornerY + m_nSizeY;

	// Сначала отрисовываем сплошной фон
	m_DC.FillSolidRect(m_nCornerX, m_nCornerY, m_nSizeX, m_nSizeY, m_dwBackGroundColor);

	// Отрисовываем границы
	{
		CPen PenBorder;
		PenBorder.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		m_DC.SelectObject(&PenBorder);

		m_DC.MoveTo(m_nCornerX, m_nCornerY);
		m_DC.LineTo(m_nCornerX + m_nSizeX, m_nCornerY);
		m_DC.LineTo(m_nCornerX + m_nSizeX, m_nCornerY + m_nSizeY);
		m_DC.LineTo(m_nCornerX, m_nCornerY + m_nSizeY);
		m_DC.LineTo(m_nCornerX, m_nCornerY);
	}

	// Отрисовываем сетку по X c шагом CELL_SIZE_X
	bool bBold = true;
	CPen BoldPenGrid;
	CPen PenGrid;

	BoldPenGrid.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	PenGrid.CreatePen(PS_SOLID, 1, RGB(150, 150, 150));

	for (int x = nStartX + CELL_SIZE_X; x < m_nSizeX + nStartX; x += CELL_SIZE_X)
	{
		m_DC.MoveTo(x, nStartY - 1);

		if (bBold)
		{
			m_DC.SelectObject(&BoldPenGrid);
			bBold = false;
		}
		else
		{
			m_DC.SelectObject(&PenGrid);
			bBold = true;
		}
		m_DC.LineTo(x, nStartY - m_nSizeY - 1);	
	}
	bBold = true;

	// Отрисовываем сетку по Y c шагом CELL_SIZE_Y
	for (int y = m_nCornerY + m_nSizeY - CELL_SIZE_Y; y > m_nCornerY; y -= CELL_SIZE_Y)
	{
		m_DC.MoveTo(nStartX, y);

		if (bBold)
		{
			m_DC.SelectObject(&BoldPenGrid);
			bBold = false;
		}
		else
		{
			m_DC.SelectObject(&PenGrid);
			bBold = true;
		}
		m_DC.LineTo(nStartX + m_nSizeX, y);

	}	

	// Отрисовываем оси
	CPen AxisPen;
	AxisPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	m_DC.SelectObject(&AxisPen);

	m_DC.MoveTo(m_nCornerX, m_nCornerY + m_nSizeY / 2);
	m_DC.LineTo(m_nCornerX + m_nSizeX, m_nCornerY + m_nSizeY / 2);

	m_DC.MoveTo(m_nCornerX + m_nSizeX / 2, m_nCornerY);
	m_DC.LineTo(m_nCornerX + m_nSizeX / 2, m_nCornerY + m_nSizeY);

	// Устанавливаем цвет фона текста
	COLORREF TextBackGround = RGB(240, 240, 240);
	m_DC.SetBkColor(TextBackGround);

	// Отрисовываем именa осей
	CString sTextX(m_sXname.c_str());
	CString sTextY(m_sYname.c_str());

	m_DC.SetTextAlign(TA_LEFT);
	m_DC.TextOut(nStartX, m_nCornerY - 20, sTextY);
	m_DC.TextOut(nStartX + m_nSizeX - 30, nStartY + 17, sTextX);

	// Отрисовываем цену деления для первого графика
	if (m_Data.empty())
		return;

	// Получаем точки по X & Y
	TData& Data = m_Data.begin()->second;
	if (Data.empty())
		return;
	std::list< int > XData;
	std::list< int > YData;
	TData::const_iterator it = Data.begin();
	const TData::const_iterator itEnd = Data.end();
	for (; it != itEnd; ++it)
	{
		XData.push_back(it->x);
		YData.push_back(it->y);
	}

	// Определяем максимальные и минимальные точки
	const int nXMax = *std::max_element(XData.begin(), XData.end());
	const int nXMin = *std::min_element(XData.begin(), XData.end());
	const int nYMax = *std::max_element(YData.begin(), YData.end());
	const int nYMin = *std::min_element(YData.begin(), YData.end());

	// Определяем сколько клеток на графике доступно для отрисовки
	const int nCellsX = m_nSizeX / CELL_SIZE_X - 2;
	const int nCellsY = m_nSizeY / CELL_SIZE_Y - 2;

	// Определяем размах значений
	const int nDeltaX = nXMax - nXMin;
	const int nDeltaY = nYMax - nYMin;

	// Определяем сколько значений данных помещается в одну клетку
	const float nPointsPerCellX = static_cast< float > (nDeltaX) / nCellsX;
	const float nPointsPerCellY = static_cast< float > (nDeltaY) / nCellsY;

	int nCostX = nXMin;

	// Отрисовываем цену деления по Х
	for (int x = nStartX; x <= m_nSizeX + nStartX; x += CELL_SIZE_X * 4)
	{
		_tstring sCount = boost::lexical_cast< _tstring > (nCostX);
		CString sText(sCount.c_str());
		m_DC.TextOut(x - 7, nStartY + 2, sText);
		nCostX += nPointsPerCellX * 4;
	}	

	m_DC.SetTextAlign(TA_RIGHT);
	int nCostY = nYMin;
	// Отрисовываем цену деления по Y
	for (int y = m_nCornerY + m_nSizeY; y >= m_nCornerY; y -= CELL_SIZE_Y * 4)
	{
		_tstring sCount = _T("  ");
		sCount += boost::lexical_cast< _tstring > (nCostY);
		CString sText(sCount.c_str());
		nCostY += nPointsPerCellY * 4;
		m_DC.TextOut(nStartX - 5, y - 7, sText);
	}
	
}

void CChart::PaintData()
{
	if (m_Data.empty())
		return;

	// Расстояние до мыши от точки графика
	int nMouseLength = m_nSizeY * m_nSizeX;

	// Точка на графике ближайшая к мыши
	CPoint GraphClosestPoint;

	// Значение точки на графике, ближайшей к мыши
	CPoint ValueClosestPoint;

	// Попадает ли мышь в область графика
	const bool bIsMouseInArea = IsMouseInChartArea();

	if (!bIsMouseInArea)
	{
		m_dwActiveChartColor = RGB(255, 255, 255);
		m_pointActivePoint.x = 0;
		m_pointActivePoint.y = 0;
	}


	// Координаты мыши
	POINT Mouse;
	GetCursorPos(&Mouse);

	// Координаты окна
	CRect Window;
	m_hWindow->GetWindowRect(Window);

	// Координаты мыши относительно окна
	const int nMouseX = Mouse.x - Window.left;
	const int nMouseY = Mouse.y - Window.top - 40;

	// Цикл по всем данным
	TAllData::const_iterator itData = m_Data.begin();
	const TAllData::const_iterator itDataEnd = m_Data.end();
	for (; itData != itDataEnd; ++itData)
	{
		const TData& Data = itData->second;
		if (Data.empty())
			continue;

		// Устанавливаем цвет кисти для текущего графика
		CPen Pen;
		Pen.CreatePen(PS_SOLID, 1, itData->first);
		m_DC.SelectObject(&Pen);	

		// Получаем точки по X & Y
		std::list< int > XData;
		std::list< int > YData;
		{
			TData::const_iterator it = Data.begin();
			const TData::const_iterator itEnd = Data.end();
			for (; it != itEnd; ++it)
			{
				XData.push_back(it->x);
				YData.push_back(it->y);
			}
		}
	
		// Определяем максимальные и минимальные точки
		const int nXMax = *std::max_element(XData.begin(), XData.end());
		const int nXMin = *std::min_element(XData.begin(), XData.end());
		const int nYMax = *std::max_element(YData.begin(), YData.end());
		const int nYMin = *std::min_element(YData.begin(), YData.end());

		// Определяем сколько пикселей на графике доступно для отрисовки
		const int nPixelsX = m_nSizeX - 2 * CELL_SIZE_X;
		const int nPixelsY = m_nSizeY - 2 * CELL_SIZE_Y;

		// Определяем размах значений
		const int nDeltaX = nXMax - nXMin;
		const int nDeltaY = nYMax - nYMin;

		// Определяем коэффициент, сколько значений графика приходится на 1 пиксель
		const double nPointsPerPixelX = static_cast< double > (nDeltaX) / nPixelsX;
		const double nPointsPerPixelY = static_cast< double > (nDeltaY) / nPixelsY;

		// Определяем точки отсчета
		const int nStartX = nPointsPerPixelX ? m_nCornerX + m_nSizeX / 2 - (nXMax + nXMin) / (2 * nPointsPerPixelX) : m_nCornerX + m_nSizeX / 2;
		const int nStartY = nPointsPerPixelY ? m_nCornerY + m_nSizeY / 2 + (nYMax + nYMin) / (2 * nPointsPerPixelY) : m_nCornerY + m_nSizeY / 2;

		// Цикл по всем значениям текущего графика
		TData::const_iterator it = Data.begin();
		const TData::const_iterator itEnd = Data.end();
		while(true)
		{
			if (it == itEnd)
				break;

			// Первая точка
			const CPoint& First = *it;
			const int nFirstX = nPointsPerPixelX ? nStartX + it->x / nPointsPerPixelX : nStartX;
			const int nFirstY = nPointsPerPixelY ? nStartY - it->y / nPointsPerPixelY : nStartY;

			// Если мышь в области графика определяем расстояние до ближайшей точки
			if (bIsMouseInArea)
			{
				const int nLength = sqrt(static_cast< float > (pow(static_cast< float > (nFirstX - nMouseX), 2) + pow(static_cast< float > (nFirstY - nMouseY), 2)));
				if (nLength < nMouseLength)
				{
					nMouseLength			= nLength;
					GraphClosestPoint.x		= nFirstX;
					GraphClosestPoint.y		= nFirstY;

					ValueClosestPoint.x		= it->x;
					ValueClosestPoint.y		= it->y;

					m_dwActiveChartColor	= itData->first;
					m_pointActivePoint		= ValueClosestPoint;
				}
			}
			++it;
			if (it == itEnd)
				break;

			// Вторая точка
			const CPoint& Second = *it;
			const int nSecondX = nPointsPerPixelX ? nStartX + it->x / nPointsPerPixelX : nStartX;
			const int nSecondY = nPointsPerPixelY ? nStartY - it->y / nPointsPerPixelY : nStartY;

			// Выполняем отрисовку линии отолько в случае если линия начинается не в координате 0, 0
			if (First.x && First.y)
			{
				// Устанавливаем координату в первую точку
				m_DC.MoveTo(nFirstX, nFirstY);

				// Рисуем линию к второй точке
				m_DC.LineTo(nSecondX, nSecondY);
			}
		}
	}

	// Получем текст тултипа
	_tstring sText = _T("x=");
	sText += boost::lexical_cast< _tstring > (ValueClosestPoint.x);
	sText += _T(" y=");
	sText += boost::lexical_cast< _tstring > (ValueClosestPoint.y);

	PaintTooltip(GraphClosestPoint, sText);
}
