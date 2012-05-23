#ifndef DIRECTEDSEGMENT_H
#define DIRECTEDSEGMENT_H
#include "NumericPair.h"

namespace Temporal
{
	class DirectedSegment
	{
	public:
		DirectedSegment(float x1, float y1, float x2, float y2) : _origin(x1, y1), _vector(x2 - x1, y2 - y1) {}
		DirectedSegment(const Point& origin, const Vector& vector) : _origin(origin), _vector(vector) {}
		const Point& getOrigin(void) const { return _origin; }
		const Vector& getVector(void) const { return _vector; }
		Point getTarget(void) const { return getOrigin() + getVector(); }
	private:
		Point _origin;
		Vector _vector;
	};

	inline DirectedSegment DirectedSegmentPP(const Point& p1, const Point& p2) { return DirectedSegment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }

}
#endif