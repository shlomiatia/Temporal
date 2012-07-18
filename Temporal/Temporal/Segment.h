#ifndef SEGMENT_H
#define SEGMENT_H
#include "NumericPair.h"
#include "Shape.h"

namespace Temporal
{
	class Segment : public Shape
	{
	public:
		static const Segment Zero;

		Segment(float x1, float y1, float x2, float y2) : _center((x1+x2)/2.0f, (y1+y2)/2.0f), _radius((x2-x1)/2.0f, (y2-y1)/2.0f) {}
		Segment(const Point& point1, const Point& point2) : _center((point1+point2)/2.0f), _radius((point2-point1)/2.0f) {}
		Segment(const Point& center, const Vector& radius);
		// No need for virtual destructor

		ShapeType::Enum getType() const { return ShapeType::SEGMENT; }

		const Point& getCenter() const { return _center; }
		const Vector& getRadius() const { return _radius; }
		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }
		float getRadiusVx() const { return getRadius().getVx(); }
		float getRadiusVy() const { return getRadius().getVy(); }

		Point getLeftPoint() const { return getCenter() + (getRadiusVx() <= 0.0f ? getRadius() : -getRadius()); }
		Point getRightPoint() const { return getCenter() + (getRadiusVx() > 0.0f ? getRadius() : -getRadius()); }
		Point getBottomPoint() const { return getCenter() + (getRadiusVy() <= 0.0f ? getRadius() : -getRadius()); }
		Point getTopPoint() const { return getCenter() + (getRadiusVy() > 0.0f ? getRadius() : -getRadius()); }
		Point getNaturalOrigin() const { return getRadiusVx() == 0.0f ? getBottomPoint() : getLeftPoint(); }
		Point getNaturalTarget() const { return getRadiusVx() == 0.0f ? getTopPoint() : getRightPoint(); }
		Vector getNaturalVector() const { return getNaturalTarget() - getNaturalOrigin(); }
		
		float getLeft() const { return getLeftPoint().getX(); }
		float getRight() const { return getRightPoint().getX(); }
		float getBottom() const { return getBottomPoint().getY(); }
		float getTop() const { return getTopPoint().getY(); }
		float getLength() const;
		float getY(float x) const { return get(Axis::Y, x); }
		float getX(float y) const  { return get(Axis::X, y); }

	private:
		Point _center;
		Vector _radius;

		float get(Axis::Enum axis, float otherAxisValue) const;
	};

	inline Segment SegmentPP(const Point& p1, const Point& p2) { return Segment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }

}
#endif