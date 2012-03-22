#ifndef RECT_H
#define RECT_H
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Segment.h"

namespace Temporal
{
	static float getOffset(float size) { return (size - 1.0f) / 2.0f; }

	class Rect
	{
	public:
		static const Rect Empty;

		Rect(float centerX, float centerY, float getWidth, float getHeight);
		Rect(const Point& center, const Size& size);

		const Point& getCenter(void) const { return (_center); }
		const Size& getSize(void) const { return (_size); }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }

		float getOffsetX(void) const { return getOffset(getWidth()); }
		float getOffsetY(void) const { return getOffset(getHeight()); }

		float getBottom(void) const { return getCenterY() - getOffsetY(); }
		float getLeft(void) const {	return getCenterX() - getOffsetX(); }
		float getTop(void) const { return getCenterY() + getOffsetY(); }
		float getRight(void) const { return getCenterX() + getOffsetX();}
		float getWidth(void) const { return _size.getWidth(); }
		float getHeight(void) const { return _size.getHeight(); }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		Rect move(const Vector& vector) const { return Rect(getCenter() + vector, getSize()); }
		Rect resize(const Vector& delta) const { return Rect(getCenter(), getSize() + delta); }
		
		bool intersectsInclusive(const Rect& rect) const { return getLeft() <= rect.getRight() && getRight() >= rect.getLeft() && getBottom() <= rect.getTop() && getTop() >= rect.getBottom(); }
		bool intersectsExclusive(const Rect& rect) const { return getLeft() < rect.getRight() && getRight() > rect.getLeft() && getBottom() < rect.getTop() && getTop() > rect.getBottom(); }
		bool contains(const Point& point) const { return getLeft() <= point.getX() && getRight() >= point.getX() && getBottom() <= point.getY() && getTop() >= point.getY(); }

		// TODO: Not here! PHYSICS
		bool intersects(const DirectedSegment& directedSegment, Point& pointOfIntersection) const;
	private:
		Point _center;
		Size _size;

		void validate(void) const;
	};

	inline Rect RectLB(float left, float bottom, float width, float height) { return Rect(left + getOffset(width), bottom + getOffset(height), width, height); }
	inline Rect RectCB(float centerX, float bottom, float width, float height) { return Rect(centerX, bottom + getOffset(height), width, height); }
	inline Rect RectLC(float left, float centerY, float width, float height) { return Rect(left + getOffset(width), centerY, width, height); }
}
#endif