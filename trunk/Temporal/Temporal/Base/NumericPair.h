#ifndef NUMERICPAIR_H
#define NUMERICPAIR_H

#include "BaseEnums.h"

namespace Temporal
{
	// Abstract
	class NumericPair
	{
	public:
		static const NumericPair Zero;

		NumericPair(float n1, float n2) : _n1(n1), _n2(n2) {}
		// No virtual destructor

		float getAxis(Axis::Enum axis) const { return axis == Axis::X ? _n1 : _n2; }
		void setAxis(Axis::Enum axis, float value) { if(axis == Axis::X) _n1 = value; else _n2 = value; }

		NumericPair operator-(void) const { return NumericPair(-_n1, -_n2); }
		bool operator==(const NumericPair& numericPair) const { return ((_n1 == numericPair._n1) && (_n2 == numericPair._n2)); }
		bool operator!=(const NumericPair& numericPair) const { return !(*this == numericPair); }
		NumericPair operator+(const NumericPair& numericPair) const {	return (NumericPair(_n1 + numericPair._n1, _n2 + numericPair._n2)); }
		NumericPair operator-(const NumericPair& numericPair) const {	return (NumericPair(_n1 - numericPair._n1, _n2 - numericPair._n2)); }
		NumericPair& operator+=(const NumericPair& numericPair)
		{
			_n1 += numericPair._n1;
			_n2 += numericPair._n2;
			return *this;
		}
		NumericPair& operator-=(const NumericPair& numericPair)
		{
			_n1 -= numericPair._n1;
			_n2 -= numericPair._n2;
			return *this;
		}
		NumericPair& operator+=(float scalar)
		{
			_n1 += scalar;
			_n2 += scalar;
			return *this;
		}
		NumericPair& operator-=(float scalar)
		{
			return *this += -scalar;
		}
		NumericPair& operator*=(float scalar)
		{
			_n1 *= scalar;
			_n2 *= scalar;
			return *this;
		}
		NumericPair& operator/=(float scalar)
		{
			return *this *= (1.0f / scalar);
		}

		friend inline NumericPair operator+(const NumericPair& numericPair, float scalar);
		friend inline NumericPair operator*(const NumericPair& numericPair, float scalar);
	protected:
		float _n1;
		float _n2;
	};

	inline NumericPair operator+(const NumericPair& numericPair, float scalar) { return NumericPair(numericPair._n1 + scalar, numericPair._n2 + scalar); }
	inline NumericPair operator+(float scalar, const NumericPair& numericPair) { return numericPair + scalar; }
	inline NumericPair operator-(const NumericPair& numericPair, float scalar) { return numericPair + -scalar; }
	inline NumericPair operator*(const NumericPair& numericPair, float scalar) { return NumericPair(numericPair._n1 * scalar, numericPair._n2 * scalar); }
	inline NumericPair operator*(float scalar, const NumericPair& numericPair) { return numericPair * scalar; }
	inline NumericPair operator/(const NumericPair& numericPair, float scalar) { return numericPair * (1.0f / scalar); }

	class Point : public NumericPair
	{
	public:
		Point(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Point(float x, float y) : NumericPair(x, y) {}

		float getX(void) const { return (_n1); }
		void setX(float n1) { _n1 = n1; }
		float getY(void) const { return (_n2); }
		void setY(float n2) { _n2 = n2; }
	};

	class Vector : public NumericPair
	{
	public:
		Vector(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Vector(float x, float y) : NumericPair(x, y) {}

		float getVx(void) const { return (_n1); }
		void setVx(float n1) { _n1 = n1; }
		float getVy(void) const { return (_n2); }
		void setVy(float n2) { _n2 = n2; }

		float getLength(void) const;

		// Angle is between -180 to 180 (-PI to PI)
		float getAngle(void) const;
		Vector normalize(void) const;
		Vector getRightNormal(void) const { return Vector(-getVy(), getVx()); }
		Vector getLeftNormal(void) const { return Vector(getVy(), -getVx()); }

		float operator*(const Vector& other) const { return getVx() * other.getVx() + getVy() * other.getVy(); }
	};

	class Size : public NumericPair
	{
	public:
		Size(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Size(float width, float height) : NumericPair(width, height) {}

		float getWidth(void) const { return (_n1); }
		void setWidth(float width) { _n1 = width; }
		float getHeight(void) const { return (_n2); }
		void setHeight(float height) { _n2 = height; }
	};

	class Range : public NumericPair
	{
	public:
		Range(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Range(float min, float max) : NumericPair(min, max) {}

		float getMin(void) const { return (_n1); }
		void setMin(float min) { _n1 = min; }
		float getMax(void) const { return (_n2); }
		void setMax(float max) { _n2 = max; }
	};
}
#endif