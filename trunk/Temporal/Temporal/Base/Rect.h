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
		Vector getRadius(void) const { return getSize() / 2.0f; }

		float getBottom(void) const { return getCenterY() - getRadius().getVy(); }
		float getLeft(void) const {	return getCenterX() - getRadius().getVx(); }
		float getTop(void) const { return getCenterY() + getRadius().getVy(); }
		float getRight(void) const { return getCenterX() + getRadius().getVx(); }
		float getWidth(void) const { return _size.getWidth(); }
		float getHeight(void) const { return _size.getHeight(); }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		Rect move(const Vector& vector) const { return Rect(getCenter() + vector, getSize()); }
		Rect resize(const Vector& delta) const { return Rect(getCenter(), getSize() + delta); }
		
		bool intersects(const Rect& rect) const { return getLeft() <= rect.getRight() && getRight() >= rect.getLeft() && getBottom() <= rect.getTop() && getTop() >= rect.getBottom(); }
		bool contains(const Point& point) const { return getLeft() <= point.getX() && getRight() >= point.getX() && getBottom() <= point.getY() && getTop() >= point.getY(); }

		bool intersects(const DirectedSegment& directedSegment, Point& pointOfIntersection) const;
		bool intersects(const Segment& segment) const;
	private:
		Point _center;
		Size _size;

		void validate(void) const;
	};

	inline Rect RectLB(float left, float bottom, float width, float height) { return Rect(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline Rect RectCB(float centerX, float bottom, float width, float height) { return Rect(centerX, bottom + height / 2.0f, width, height); }
	inline Rect RectLC(float left, float centerY, float width, float height) { return Rect(left + width / 2.0f, centerY, width, height); }
}
#endif