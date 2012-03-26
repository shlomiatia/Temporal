#ifndef SEGMENT_H
#define SEGMENT_H
#include "NumericPair.h"

namespace Temporal
{
	class Segment
	{
	public:
		Segment(float x1, float y1, float x2, float y2) : _point1(x1, y1), _point2(x2, y2) {};
		Segment(const Point& p1, const Point& p2) : _point1(p1), _point2(p2) {};

		const Point& getPoint1(void) const { return _point1; }
		const Point& getPoint2(void) const { return _point2; }

		float getLength(void) const;
	private:
		const Point _point1;
		const Point _point2;
	};

	class DirectedSegment : public Segment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY) : Segment(originX, originY, targetX, targetY) {};
		DirectedSegment(const Point& origin, const Point& target) : Segment(origin, target) {};

		const Point& getOrigin(void) const { return getPoint1(); }
		const Point& getTarget(void) const { return getPoint2(); }
		
		Vector getVector(void) const;
		float getAngle(void) const;

	private:
	};
}
#endif