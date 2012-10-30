#ifndef VECTOR_H
#define VECTOR_H

#include "BaseEnums.h"

namespace Temporal
{
	class SerializationAccess;

	class Vector
	{
	public:
		static const Vector Zero;

		Vector() : _x(0.0f), _y(0.0f) {}
		Vector(float x, float y) : _x(x), _y(y) {}

		float getX() const { return (_x); }
		void setX(float x) { _x = x; }
		float getY() const { return (_y); }
		void setY(float y) { _y = y; }

		float getAxis(Axis::Enum axis) const { return axis == Axis::X ? getX() : getY(); }
		void setAxis(Axis::Enum axis, float value) { if(axis == Axis::X) setX(value); else setY(value); }

		// Angle is between -180 to 180 (-PI to PI)
		float getAngle() const;
		float getLength() const;
		Vector normalize() const;
		Vector getLeftNormal() const { return Vector(-getY(), getX()); }
		Vector getRightNormal() const { return Vector(getY(), -getX()); }
		Vector absolute() const;

		Vector operator-() const { return Vector(-getX(), -getY()); }
		bool operator==(const Vector& vector) const { return ((getX() == vector.getX()) && (getY() == vector.getY())); }
		bool operator!=(const Vector& vector) const { return !(*this == vector); }
		Vector operator+(const Vector& vector) const {	return (Vector(getX() + vector.getX(), getY() + vector.getY())); }
		Vector operator-(const Vector& vector) const {	return (Vector(getX() - vector.getX(), getY() - vector.getY())); }
		float operator*(const Vector& other) const { return getX() * other.getX() + getY() * other.getY(); }
		Vector& operator+=(const Vector& vector)
		{
			_x += vector._x;
			_y += vector._y;
			return *this;
		}
		Vector& operator-=(const Vector& vector)
		{
			_x -= vector._x;
			_y -= vector._y;
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

		friend class SerializationAccess;
	};

	inline Vector operator+(const Vector& vector, float scalar) { return Vector(vector.getX() + scalar, vector.getY() + scalar); }
	inline Vector operator+(float scalar, const Vector& vector) { return vector + scalar; }
	inline Vector operator-(const Vector& vector, float scalar) { return vector + -scalar; }
	inline Vector operator*(const Vector& vector, float scalar) { return Vector(vector.getX() * scalar, vector.getY() * scalar); }
	inline Vector operator*(float scalar, const Vector& vector) { return vector * scalar; }
	inline Vector operator/(const Vector& vector, float scalar) { return vector * (1.0f / scalar); }

	class Size
	{
	public:
		static const Size Zero;

		Size(float width, float height) : _width(width), _height(height) {}

		float getWidth() const { return _width; }
		void setWidth(float width) { _width = width; }
		float getHeight() const { return _height; }
		void setHeight(float height) { _height = height; }

		Vector toVector() const { return Vector(getWidth(), getHeight()); }

	private:
		float _width;
		float _height;
	};
}
#endif