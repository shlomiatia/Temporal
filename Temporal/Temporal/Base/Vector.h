#ifndef VECTOR_H
#define VECTOR_H

#include "BaseEnums.h"
#include <math.h>

namespace Temporal
{
	// TODO: Divide SLOTH
	class Vector
	{
	public:
		static const Vector Zero;

		Vector(float x, float y) : _x(x), _y(y) {}

		// Point, Vector
		float getX(void) const { return (_x); }
		void setX(float x) { _x = x; }
		float getY(void) const { return (_y); }
		void setY(float y) { _y = y; }

		// Size
		float getWidth(void) const { return (_x); }
		void setWidth(float width) { _x = width; }
		float getHeight(void) const { return (_y); }
		void setHeight(float height) { _y = height; }

		// Range
		float getMin(void) const { return (_x); }
		void setMin(float min) { _x = min; }
		float getMax(void) const { return (_y); }
		void setMax(float max) { _y = max; }

		float getAxis(Axis::Enum axis) const { return axis == Axis::X ? getX() : getY(); }
		void setAxis(Axis::Enum axis, float value) { if(axis == Axis::X) setX(value); else setY(value); }

		Vector operator-(void) const { return Vector(-getX(), -getY()); }
		bool operator==(const Vector& vector) const { return ((getX() == vector.getX()) && (getY() == vector.getY())); }
		bool operator!=(const Vector& vector) const { return !(*this == vector); }
		Vector operator+(const Vector& vector) const {	return (Vector(getX() + vector.getX(), getY() + vector.getY())); }
		Vector operator-(const Vector& vector) const {	return (Vector(getX() - vector.getX(), getY() - vector.getY())); }
		Vector& operator+=(const Vector& vector)
		{
			_x += vector.getX();
			_y += vector.getY();
			return *this;
		}
		Vector& operator-=(const Vector& vector)
		{
			_x -= vector.getX();
			_y -= vector.getY();
			return *this;
		}
		Vector& operator+=(float scalar)
		{
			_x += scalar;
			_y += scalar;
			return *this;
		}
		Vector& operator-=(float scalar)
		{
			return *this += -scalar;
		}
		Vector& operator*=(float scalar)
		{
			_x *= scalar;
			_y *= scalar;
			return *this;
		}
		Vector& operator/=(float scalar)
		{
			return *this *= (1.0f / scalar);
		}
	private:
		float _x;
		float _y;
	};

	inline Vector operator+(const Vector& vector, float scalar) { return Vector(vector.getX() + scalar, vector.getY() + scalar); }
	inline Vector operator-(const Vector& vector, float scalar) { return vector + -scalar; }
	inline Vector operator*(const Vector& vector, float scalar) { return Vector(vector.getX() * scalar, vector.getY() * scalar); }
	inline Vector operator/(const Vector& vector, float scalar) { return vector * (1.0f / scalar); }
}
#endif