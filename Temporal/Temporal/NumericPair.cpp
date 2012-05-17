#include "NumericPair.h"
#include <math.h>

namespace Temporal
{
	const NumericPair NumericPair::Zero(0.0f, 0.0f);

	NumericPair NumericPair::absolute(void) const
	{
		return NumericPair(abs(_n1), abs(_n2));
	}

	float Vector::getLength(void) const
	{
		return sqrt(pow(getVx(), 2.0f) + pow(getVy(), 2.0f));
	}

	float Vector::getAngle(void) const
	{
		return atan2(getVy(), getVx());
	}

	Vector Vector::normalize(void) const
	{
		return *this / getLength();
	}
}