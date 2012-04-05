#ifndef RECT_H
#define RECT_H
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Segment.h"
#include <stdio.h>

namespace Temporal
{
	class Rect
	{
	public:
		static const Rect Empty;

		Rect(float centerX, float centerY, float width, float height);
		Rect(const Point& center, const Size& size);
		Rect(const Point& center, const Vector& radius);

		const Point& getCenter(void) const { return _center; }
		const Vector& getRadius(void) const { return _radius; }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }

		float getRadiusVx(void) const { return getRadius().getVx(); }
		float getRadiusVy(void) const { return getRadius().getVy(); }

		float getBottom(void) const { return getCenterY() - getRadiusVy(); }
		float getLeft(void) const {	return getCenterX() - getRadiusVx(); }
		float getTop(void) const { return getCenterY() + getRadiusVy(); }
		float getRight(void) const { return getCenterX() + getRadiusVx(); }

		float getWidth(void) const { return getRadiusVx() * 2.0f; }
		float getHeight(void) const { return getRadiusVy() * 2.0f; }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		Rect move(const Vector& vector) const { return Rect(getCenter() + vector, getRadius()); }
		Rect resize(const Vector& delta) const { return Rect(getCenter(), Vector(getRadius() + delta / 2.0f)); }
		
		bool intersects(const Rect& rect) const;
		bool intersects(const Point& point) const;
		bool intersects(const Segment& segment, Vector* correction = NULL) const;
		bool intersects(const DirectedSegment& directedSegment, Point* pointOfIntersection = NULL) const;
	private:
		Point _center;
		Vector _radius;

		void validate(void) const;
	};

	inline Rect RectLB(float left, float bottom, float width, float height) { return Rect(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline Rect RectCB(float centerX, float bottom, float width, float height) { return Rect(centerX, bottom + height / 2.0f, width, height); }
	inline Rect RectLC(float left, float centerY, float width, float height) { return Rect(left + width / 2.0f, centerY, width, height); }
}
#endif