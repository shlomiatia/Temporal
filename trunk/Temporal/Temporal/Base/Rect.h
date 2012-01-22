#pragma once
#include "Enums.h"
#include "Vector.h"
#include <assert.h>

namespace Temporal
{
	class Rect
	{
	public:
		Rect(const float centerX, const float centerY, const float getWidth, const float getHeight)
			: _center(centerX, centerY), _size(getWidth, getHeight)
		{
			assert(getSize().getWidth() > 0);
			assert(getSize().getHeight() > 0);
		}
		Rect(const Vector& center, const Vector& size)
			: _center(center), _size(size)
		{
			assert(getSize().getWidth() > 0);
			assert(getSize().getHeight() > 0);
		}

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

		float getGraphicsLeft(void) const { return getLeft(); }
		float getGraphicsRight(void) const { return getRight() + 1.0f; }
		float getGraphicsTop(void) const { return getBottom(); }
		float getGraphicsBottom(void) const { return getTop() + 1.0f; }

		float getSide(Orientation::Type orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Type orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		Rect operator+(const Vector& vec) const { return (Rect(getCenter() + vec, getSize())); }
		Rect operator-(const Vector& vec) const { return (Rect(getCenter() - vec, getSize())); }
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
		bool intersectsInclusive(const Rect& rect) const { return ((getLeft() <= rect.getRight()) && (getRight() >= rect.getLeft()) && (getBottom() <= rect.getTop()) && (getTop() >= rect.getBottom())); }
		bool intersectsExclusive(const Rect& rect) const { return ((getLeft() < rect.getRight()) && (getRight() > rect.getLeft()) && (getBottom() < rect.getTop()) && (getTop() > rect.getBottom())); }
	private:
		Vector _center;
		Vector _size;
	};
}