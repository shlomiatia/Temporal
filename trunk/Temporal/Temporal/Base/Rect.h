#pragma once
#include "Enums.h"
#include "Vector.h"

namespace Temporal
{
	class Rect
	{
	public:
		Rect(float centerX, float centerY, float getWidth, float getHeight);
		Rect(const Vector& center, const Vector& size);

		const Vector& getCenter(void) const { return (_center); }
		const Vector& getSize(void) const { return (_size); }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }

		float getOffsetX(void) const { return (getWidth() - 1.0f) / 2.0f; }
		float getOffsetY(void) const { return (getHeight() - 1.0f) / 2.0f; }

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
		bool contains(float x, float y) const { return getLeft() <= x && getRight() >= x && getBottom() <= y && getTop() >= y; }
	private:
		Vector _center;
		Vector _size;
	};

	inline Rect operator+(const Rect& rect, const Vector& vec) { return (Rect(rect.getCenter() + vec, rect.getSize())); }
	inline Rect operator-(const Rect& rect, const Vector& vec) { return (Rect(rect.getCenter() - vec, rect.getSize())); }
}