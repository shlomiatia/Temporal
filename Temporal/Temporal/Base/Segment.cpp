#include "Segment.h"
#include <math.h>

namespace Temporal
{
	float DirectedSegment::getLength(void) const 
	{
		return sqrt(pow(_target.getX() - _origin.getX(), 2.0f) + pow(_target.getY() - _origin.getY(), 2.0f));
	}
	float DirectedSegment::getAngle(void) const
	{
		float slope = (_target.getY() - _origin.getY()) / (_target.getX() - _origin.getX());
		float angle = atan(slope);
		return angle;
	}
}