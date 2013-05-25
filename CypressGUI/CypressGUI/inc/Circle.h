#pragma once

#include <math.h>
#include <algorithm>
#include <vector>
#include <exception>

//! Класс - реализация точки на плоскости
struct CPOINT
{
	CPOINT() :
		x(0),
		y(0)
	{
	}
	double x;
	double y;
};

//! Класс для работы с окружностями
class CCircle
{
public:

	//! Конструктор
	CCircle(CPOINT, CPOINT, CPOINT);

	//! Получить радиус
	const double GetRadius() const;

	//! Получить центр
	const CPOINT GetCenter() const;

	//! Найти точки пересечения с другой окружностью
	const std::vector< CPOINT > GetCrossPoints(CCircle&) const;

	//! Check the given CPOINT are perpendicular to x or y axis 
	bool IsPerpendicular(CPOINT pt1, CPOINT pt2, CPOINT pt3);

private:

	//! Расчитать окружность
	void Calc(CPOINT P1, CPOINT P2, CPOINT P3);

	//! Центр окружности
	CPOINT m_Center;

	//! Радиус окружности
	double m_nRadius;

};
