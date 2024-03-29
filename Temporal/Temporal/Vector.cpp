#include "Vector.h"
#include <math.h>

namespace Temporal
{
	const Vector Vector::Zero(0.0f, 0.0f);

	Vector::Vector(float angle)
		: _x(cos(angle)), _y(sin(angle))
	{
	}

	Vector Vector::absolute() const
	{
		return Vector(fabsf(_x), fabsf(_y));
	}

	float Vector::getLength() const
	{
		return sqrt(pow(getX(), 2.0f) + pow(getY(), 2.0f));
	}

	float Vector::getAngle() const
	{
		return atan2(getY(), getX());
	}

	Vector Vector::normalize() const
	{
		float length = getLength();
		return length == 0.0f ? Vector::Zero : *this / length;
	}
}