#pragma once

namespace Temporal
{
	class Vector
	{
	public:
		static const Vector Zero;

		Vector(const float x, const float y)
			: _x(x), _y(y) {}

		float getX(void) const { return (_x); }
		void setX(float x) { _x = x; }
		float getY(void) const { return (_y); }
		void setY(float y) { _y = y; }

		// Treat vector as size
		float getWidth(void) const { return (_x); }
		void setWidth(float width) { _x = width; }
		float getHeight(void) const { return (_y); }
		void setHeight(float height) { _y = height; }

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

	extern inline Vector operator+(const Vector& vector, float scalar);
	extern inline Vector operator-(const Vector& vector, float scalar);
	extern inline Vector operator*(const Vector& vector, float scalar);
	extern inline Vector operator/(const Vector& vector, float scalar);
}