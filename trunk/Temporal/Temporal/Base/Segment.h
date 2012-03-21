#ifndef SEGMENT_H
#define SEGMENT_H
#include "Vector.h"

namespace Temporal
{
	// TODO: Extract segment PHYSICS
	class DirectedSegment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY) : _origin(originX, originY), _target(targetX, targetY) {}
		DirectedSegment(const Vector& origin, const Vector& target) : _origin(origin), _target(target) {}

		const Vector& getOrigin(void) const { return _origin; }
		const Vector& getTarget(void) const { return _target; }
		Vector getVector(void) const { return getTarget() - getOrigin(); }
		float getLength(void) const;
		float getAngle(void) const;

	private:
		const Vector _origin;
		const Vector _target;
	};
}
#endif