#ifndef LINE_H
#define LINE_H
#include "Vector.h"
#include <math.h>

namespace Temporal
{
	class Segment
	{
	public:
		// TODO: Assert or remove
		Segment(float x1, float y1, float x2, float y2) : _p1(x1, y1), _p2(x2, y2) {}
		Segment(const Vector& p1, const Vector& p2) : _p1(p1), _p2(p2) {}
		// TODO: Not virtual destructor?

		const Vector& getP1(void) const { return _p1; }
		const Vector& getP2(void) const { return _p2; }

		float getLength(void) const { return sqrt(pow(_p2.getX() - _p1.getX(), 2.0f) + pow(_p2.getY() - _p1.getY(), 2.0f)); }
	protected:
		const Vector _p1;
		const Vector _p2;
	};

	class DirectedSegment : private Segment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY) : Segment(originX, originY, targetX, targetY) {}
		DirectedSegment(const Vector& origin, const Vector& target) : Segment(origin, target) {}

		const Vector& getOrigin(void) const { return _p1; }
		const Vector& getTarget(void) const { return _p2; }
		float getLength(void) const { return Segment::getLength(); }
		Vector getVector(void) const { return getTarget() - getOrigin(); }
	};
}
#endif