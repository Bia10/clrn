#include "stdafx.h"
#include "Circle.h"

CCircle::CCircle(CPOINT V1, CPOINT V2, CPOINT V3)
{
	if (!IsPerpendicular(V1, V2, V3) )				
		Calc(V1, V2, V3);	
	else if (!IsPerpendicular(V1, V3, V2) )		
		Calc(V1, V3, V2);	
	else if (!IsPerpendicular(V2, V1, V3) )		
		Calc(V2, V1, V3);	
	else if (!IsPerpendicular(V2, V3, V1) )		
		Calc(V2, V3, V1);	
	else if (!IsPerpendicular(V3, V2, V1) )		
		Calc(V3, V2, V1);	
	else if (!IsPerpendicular(V3, V1, V2) )		
		Calc(V3, V1, V2);	
	else 
	{ 
		throw std::runtime_error("The three pts are perpendicular to axis");
	}
}

const double CCircle::GetRadius() const
{
	return m_nRadius;
}

const CPOINT CCircle::GetCenter() const
{
	return m_Center;
}

const std::vector< CPOINT > CCircle::GetCrossPoints(CCircle& Circle) const
{
	// Радиус первой окружности
	const double r0 = GetRadius();

	// Радиус второй окружности
	const double r1 = Circle.GetRadius();

	// Центр первой коружности
	CPOINT P0;
	P0.x = GetCenter().x;
	P0.y = GetCenter().y;

	// Центр второй коружности
	CPOINT P1;
	P1.x = Circle.GetCenter().x;
	P1.y = Circle.GetCenter().y;

	// Алгоритм отсюда http://www.sql.ru/forum/actualthread.aspx?tid=713694
	// Расстояние между центрами окружностей
	const double D = sqrt(pow(P1.x - P0.x, 2) + pow(P1.y - P0.y, 2));
	
	// "а" на графике
	const double A = (pow(D, 2) + pow(r0, 2) - pow(r1, 2)) / (2 * D);

	// "h" на графике
	const double H = sqrt(pow(r0, 2) - pow(A, 2));

	// Точка P2
	CPOINT P2;
	P2.x = P0.x + A * (P1.x - P0.x) / D;
	P2.y = P0.y + A * (P1.y - P0.y) / D;

	// Результат - 2 точки
	std::vector< CPOINT > Result;
	CPOINT PResult;
	{
		PResult.x = P2.x + H * ( P1.y - P0.y ) / D;
		PResult.y = P2.y + H * ( P1.x - P0.x ) / D;
		Result.push_back(PResult);
	}
	{
		PResult.x = P2.x - H * ( P1.y - P0.y ) / D;
		PResult.y = P2.y - H * ( P1.x - P0.x ) / D;
		Result.push_back(PResult);
	}

	return Result; 
}

bool CCircle::IsPerpendicular(CPOINT pt1, CPOINT pt2, CPOINT pt3)
// Check the given CPOINT are perpendicular to x or y axis 
{
	double yDelta_a = pt2.y - pt1.y;
	double xDelta_a = pt2.x - pt1.x;
	double yDelta_b = pt3.y - pt2.y;
	double xDelta_b = pt3.x - pt2.x;

	// checking whether the line of the two pts are vertical
	if (fabs(xDelta_a) <= 0.000000001 && fabs(yDelta_b) <= 0.000000001)
		return false;

	if (fabs(yDelta_a) <= 0.0000001)
		return true;
	else if (fabs(yDelta_b) <= 0.0000001)
		return true;
	else if (fabs(xDelta_a)<= 0.000000001)
		return true;
	else if (fabs(xDelta_b)<= 0.000000001)
		return true;
	else return false ;
}


void CCircle::Calc(CPOINT P1, CPOINT P2, CPOINT P3)
{
	// Составляем уравнение пересечения серединных перпендикуляров вида:
	// (x2-x1)x + (y2-y1)y = (x2-x1)(x2+x1)/2 + (y2-y1)(y2+y1)/2
	// (x3-x1)x + (y3-y1)y = (x3-x1)(x3+x1)/2 + (y3-y1)(y3+y1)/2

	// Решаем систему уравнений вида:
	//  A1*x + B1*y = C1
	//	A2*x + B2*y = C2

	// x = (C1*B2 - C2*B1)/(A1*B2 - A2*B1)
	// y = (A1*C2 - A2*C1)/(A1*B2 - A2*B1)

	// Исходные данные
	const double x1 = P1.x;
	const double y1 = P1.y;

	const double x2 = P2.x;
	const double y2 = P2.y;

	const double x3 = P3.x;
	const double y3 = P3.y;

	// Коэффициенты системы уравнений
	const double A1 = x2 - x1;
	const double A2 = x3 - x1;

	const double B1 = y2 - y1;
	const double B2 = y3 - y1;

	const double C1 = (x2 - x1) * (x2 + x1) / 2 + (y2 - y1) * (y2 + y1 )/ 2;
	const double C2 = (x3 - x1) * (x3 + x1) / 2 + (y3 - y1) * (y3 + y1 )/ 2;

	// Решение системы
	m_Center.x = (C1 * B2 - C2 * B1) / (A1 * B2 - A2 * B1);
	m_Center.y = (A1 * C2 - A2 * C1) / (A1 * B2 - A2 * B1);
	m_nRadius = sqrt(pow(m_Center.x - x1, 2) + pow(m_Center.y - y1, 2));

}