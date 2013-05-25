#pragma once

#include <math.h>
#include <algorithm>
#include <vector>
#include <exception>

//! ����� - ���������� ����� �� ���������
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

//! ����� ��� ������ � ������������
class CCircle
{
public:

	//! �����������
	CCircle(CPOINT, CPOINT, CPOINT);

	//! �������� ������
	const double GetRadius() const;

	//! �������� �����
	const CPOINT GetCenter() const;

	//! ����� ����� ����������� � ������ �����������
	const std::vector< CPOINT > GetCrossPoints(CCircle&) const;

	//! Check the given CPOINT are perpendicular to x or y axis 
	bool IsPerpendicular(CPOINT pt1, CPOINT pt2, CPOINT pt3);

private:

	//! ��������� ����������
	void Calc(CPOINT P1, CPOINT P2, CPOINT P3);

	//! ����� ����������
	CPOINT m_Center;

	//! ������ ����������
	double m_nRadius;

};
