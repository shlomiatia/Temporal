#ifndef SEGMENT_H
#define SEGMENT_H
#include "Vector.h"

namespace Temporal
{
	// TODO: Extract segment PHYSICS
	class DirectedSegment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY);
		DirectedSegment(const Vector& origin, const Vector& target);

		const Vector& getOrigin(void) const { return _origin; }
		const Vector& getTarget(void) const { return _target; }
		float getLength(void) const { return _length; }
		const Vector& getNormalizedVector(void) const { return _normalizedVector; }
		float getAngle(void) const { return _angle; }

	private:
		const Vector _origin;
		const Vector _target;
		const float _length;
		const Vector _normalizedVector;
		const float _angle;

		float calculateLength(void) const;
		Vector calculateNormalizedVector(void) const;
		float calculateAngle(void) const;
	};
}
#endif