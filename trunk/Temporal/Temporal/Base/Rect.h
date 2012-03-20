#ifndef RECT_H
#define RECT_H
#include "BaseEnums.h"
#include "Vector.h"

namespace Temporal
{
	static float getOffset(float size) { return (size - 1.0f) / 2.0f; }

	class Rect
	{
	public:
		

		Rect(float centerX, float centerY, float getWidth, float getHeight);
		Rect(const Vector& center, const Vector& size);

		const Vector& getCenter(void) const { return (_center); }
		const Vector& getSize(void) const { return (_size); }

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

		Rect& operator+=(const Vector& vec)
		{
			_center += vec;
			return *this;
		}
		Rect& operator-=(const Vector& vec)
		{
			_center -= vec;
			return *this;
		}
		bool intersectsInclusive(const Rect& rect) const { return getLeft() <= rect.getRight() && getRight() >= rect.getLeft() && getBottom() <= rect.getTop() && getTop() >= rect.getBottom(); }
		bool intersectsExclusive(const Rect& rect) const { return getLeft() < rect.getRight() && getRight() > rect.getLeft() && getBottom() < rect.getTop() && getTop() > rect.getBottom(); }
		bool contains(const Vector& point) const { return getLeft() <= point.getX() && getRight() >= point.getX() && getBottom() <= point.getY() && getTop() >= point.getY(); }
	private:
		Vector _center;
		Vector _size;
	};

	inline Rect RectLB(float left, float bottom, float width, float height) { return Rect(left + getOffset(width), bottom + getOffset(height), width, height); }
	inline Rect RectCB(float centerX, float bottom, float width, float height) { return Rect(centerX, bottom + getOffset(height), width, height); }
	inline Rect RectLC(float left, float centerY, float width, float height) { return Rect(left + getOffset(width), centerY, width, height); }
	inline Rect operator+(const Rect& rect, const Vector& vec) { return (Rect(rect.getCenter() + vec, rect.getSize())); }
	inline Rect operator-(const Rect& rect, const Vector& vec) { return (Rect(rect.getCenter() - vec, rect.getSize())); }
}
#endif