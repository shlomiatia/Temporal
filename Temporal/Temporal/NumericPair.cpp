#include "NumericPair.h"
#include <math.h>

namespace Temporal
{
	const NumericPair NumericPair::Zero(0.0f, 0.0f);

	NumericPair NumericPair::absolute() const
	{
		return NumericPair(abs(_n1), abs(_n2));
	}

	float Vector::getLength() const
	{
		return sqrt(pow(getVx(), 2.0f) + pow(getVy(), 2.0f));
	}

	float Vector::getAngle() const
	{
		return atan2(getVy(), getVx());
	}

	Vector Vector::normalize() const
	{
		return *this / getLength();
	}
}