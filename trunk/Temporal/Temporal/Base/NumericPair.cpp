#include "NumericPair.h"
#include <math.h>

namespace Temporal
{
	const NumericPair NumericPair::Zero(0.0f, 0.0f);

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