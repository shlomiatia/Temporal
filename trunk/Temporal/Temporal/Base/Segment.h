#ifndef SEGMENT_H
#define SEGMENT_H
#include "NumericPair.h"

namespace Temporal
{
	class DirectedSegment;

	class Segment
	{
	public:
		static const Segment Empty;

		Segment(float x1, float y1, float x2, float y2) : _point1(x1, y1), _point2(x2, y2) {};
		Segment(const Point& p1, const Point& p2) : _point1(p1), _point2(p2) {};

		const Point& getPoint1(void) const { return _point1; }
		const Point& getPoint2(void) const { return _point2; }
		Point getCenter(void) const { return (getPoint1() + getPoint2()) / 2.0f; }
		Vector getRadius(void) const { return getPoint2() - getCenter(); }

		float getLeft(void) const { return getPoint1().getX() < getPoint2().getX() ? getPoint1().getX() : getPoint2().getX(); }
		float getRight(void) const { return getPoint1().getX() > getPoint2().getX() ? getPoint1().getX() : getPoint2().getX(); }
		float getTop(void) const { return getPoint1().getY() > getPoint2().getY() ? getPoint1().getY() : getPoint2().getY(); }
		float getBottom(void) const { return getPoint1().getY() < getPoint2().getY() ? getPoint1().getY() : getPoint2().getY(); }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		float getLength(void) const;

		bool intersects(const Segment& other, Point& pointOfIntersection) const;
	private:
		Point _point1;
		Point _point2;
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