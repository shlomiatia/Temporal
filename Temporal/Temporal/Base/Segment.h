#ifndef SEGMENT_H
#define SEGMENT_H
#include "NumericPair.h"
#include "Shape.h"

namespace Temporal
{
	class Segment : public Shape
	{
	public:
		static const Segment Empty;

		Segment(float x1, float y1, float x2, float y2) : _point1(x1, y1), _point2(x2, y2) {};
		Segment(const Point& p1, const Point& p2) : _point1(p1), _point2(p2) {};
		// No need for virtual destructor

		ShapeType::Enum getType(void) const { return ShapeType::SEGMENT; }

		const Point& getPoint1(void) const { return _point1; }
		const Point& getPoint2(void) const { return _point2; }
		const Point& getLeftPoint(void) const { return getPoint1().getX() <= getPoint2().getX() ? getPoint1() : getPoint2(); }
		const Point& getRightPoint(void) const { return getPoint1().getX() > getPoint2().getX() ? getPoint1() : getPoint2(); }
		const Point& getBottomPoint(void) const { return getPoint1().getY() <= getPoint2().getY() ? getPoint1() : getPoint2(); }
		const Point& getTopPoint(void) const { return getPoint1().getY() > getPoint2().getY() ? getPoint1() : getPoint2(); }
		const Point& getNaturalOrigin(void) const { return getPoint1().getX() == getPoint2().getX() ? getBottomPoint() : getLeftPoint(); }
		const Point& getNaturalTarget(void) const { return getPoint1().getX() == getPoint2().getX() ? getTopPoint() : getRightPoint(); }
		Vector getNaturalVector(void) const { return getNaturalTarget() - getNaturalOrigin(); }
		Point getCenter(void) const { return Point(getCenterX(), getCenterY()); }
		Vector getRadius(void) const { return Vector((getRight() - getLeft()) / 2.0f, (getTop() - getBottom()) / 2.0f); }

		float getLeft(void) const { return getLeftPoint().getX(); }
		float getRight(void) const { return getRightPoint().getX(); }
		float getBottom(void) const { return getBottomPoint().getY(); }
		float getTop(void) const { return getTopPoint().getY(); }
		float getLength(void) const;
		float getY(float x) const;
	private:
		Point _point1;
		Point _point2;
	};

	class DirectedSegment : private Segment
	{
	public:
		DirectedSegment(float originX, float originY, float targetX, float targetY) : Segment(originX, originY, targetX, targetY) {};
		DirectedSegment(const Point& origin, const Point& target) : Segment(origin, target) {};

		const Point& getOrigin(void) const { return getPoint1(); }
		const Point& getTarget(void) const { return getPoint2(); }
		
		float getLength(void) const { return Segment::getLength(); }
		Vector getVector(void) const;
		float getAngle(void) const;
	};
}
#endif