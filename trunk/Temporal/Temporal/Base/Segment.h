#ifndef SEGMENT_H
#define SEGMENT_H
#include "NumericPair.h"

namespace Temporal
{
	class DirectedSegment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY);
		DirectedSegment(const Point& origin, const Point& target);

		const Point& getOrigin(void) const { return _origin; }
		const Point& getTarget(void) const { return _target; }
		float getLength(void) const { return _length; }
		const Vector& getNormalizedVector(void) const { return _normalizedVector; }
		float getAngle(void) const { return _angle; }

	private:
		const Point _origin;
		const Point _target;
		const float _length;
		const Vector _normalizedVector;
		const float _angle;

		float calculateLength(void) const;
		Vector calculateNormalizedVector(void) const;
		float calculateAngle(void) const;
	};
}
#endif