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

		ShapeType::Enum getType(void) const { return ShapeType::SEGMENT; }

		const Point& getCenter(void) const { return _center; }
		const Vector& getRadius(void) const { return _radius; }
		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }
		float getRadiusVx(void) const { return getRadius().getVx(); }
		float getRadiusVy(void) const { return getRadius().getVy(); }

		Point getLeftPoint(void) const { return getCenter() + (getRadiusVx() <= 0.0f ? getRadius() : -getRadius()); }
		Point getRightPoint(void) const { return getCenter() + (getRadiusVx() > 0.0f ? getRadius() : -getRadius()); }
		Point getBottomPoint(void) const { return getCenter() + (getRadiusVy() <= 0.0f ? getRadius() : -getRadius()); }
		Point getTopPoint(void) const { return getCenter() + (getRadiusVy() > 0.0f ? getRadius() : -getRadius()); }
		Point getNaturalOrigin(void) const { return getRadiusVx() == 0.0f ? getBottomPoint() : getLeftPoint(); }
		Point getNaturalTarget(void) const { return getRadiusVx() == 0.0f ? getTopPoint() : getRightPoint(); }
		Vector getNaturalVector(void) const { return getNaturalTarget() - getNaturalOrigin(); }
		
		float getLeft(void) const { return getLeftPoint().getX(); }
		float getRight(void) const { return getRightPoint().getX(); }
		float getBottom(void) const { return getBottomPoint().getY(); }
		float getTop(void) const { return getTopPoint().getY(); }
		float getLength(void) const;
		float getY(float x) const { return get(Axis::Y, x); }
		float getX(float y) const  { return get(Axis::X, y); }

	private:
		Point _center;
		Vector _radius;

		float get(Axis::Enum axis, float otherAxisValue) const;
	};
}
#endif