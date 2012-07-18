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

		float getN1() const { return (_n1); }
		void setN1(float n1) { _n1 = n1; }
		float getN2() const { return (_n2); }
		void setN2(float n2) { _n2 = n2; }

		float getAxis(Axis::Enum axis) const { return axis == Axis::X ? getN1() : getN2(); }
		void setAxis(Axis::Enum axis, float value) { if(axis == Axis::X) setN1(value); else setN2(value); }

		NumericPair absolute() const;
		NumericPair operator-() const { return NumericPair(-getN1(), -getN2()); }
		bool operator==(const NumericPair& numericPair) const { return ((getN1() == numericPair.getN1()) && (getN2() == numericPair.getN2())); }
		bool operator!=(const NumericPair& numericPair) const { return !(*this == numericPair); }
		NumericPair operator+(const NumericPair& numericPair) const {	return (NumericPair(getN1() + numericPair.getN1(), getN2() + numericPair.getN2())); }
		NumericPair operator-(const NumericPair& numericPair) const {	return (NumericPair(getN1() - numericPair.getN1(), getN2() - numericPair.getN2())); }
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
	private:
		float _n1;
		float _n2;
	};

	inline NumericPair operator+(const NumericPair& numericPair, float scalar) { return NumericPair(numericPair.getN1() + scalar, numericPair.getN2() + scalar); }
	inline NumericPair operator+(float scalar, const NumericPair& numericPair) { return numericPair + scalar; }
	inline NumericPair operator-(const NumericPair& numericPair, float scalar) { return numericPair + -scalar; }
	inline NumericPair operator*(const NumericPair& numericPair, float scalar) { return NumericPair(numericPair.getN1() * scalar, numericPair.getN2() * scalar); }
	inline NumericPair operator*(float scalar, const NumericPair& numericPair) { return numericPair * scalar; }
	inline NumericPair operator/(const NumericPair& numericPair, float scalar) { return numericPair * (1.0f / scalar); }

	class Point : public NumericPair
	{
	public:
		Point(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Point(float x, float y) : NumericPair(x, y) {}

		float getX() const { return getN1(); }
		void setX(float x) { setN1(x); }
		float getY() const { return getN2(); }
		void setY(float y) { setN2(y); }
	};

	class Vector : public NumericPair
	{
	public:
		Vector(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Vector(float x, float y) : NumericPair(x, y) {}

		float getVx() const { return getN1(); }
		void setVx(float vx) { setN1(vx); }
		float getVy() const { return getN2(); }
		void setVy(float vy) { setN2(vy); }

		float getLength() const;

		// Angle is between -180 to 180 (-PI to PI)
		float getAngle() const;
		Vector normalize() const;
		Vector getLeftNormal() const { return Vector(-getVy(), getVx()); }
		Vector getRightNormal() const { return Vector(getVy(), -getVx()); }

		float operator*(const Vector& other) const { return getVx() * other.getVx() + getVy() * other.getVy(); }
	};

	class Size : public NumericPair
	{
	public:
		Size(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Size(float width, float height) : NumericPair(width, height) {}

		float getWidth() const { return getN1(); }
		void setWidth(float width) { setN1(width); }
		float getHeight() const { return getN2(); }
		void setHeight(float height) { setN2(height); }
	};

	class Range : public NumericPair
	{
	public:
		Range(const NumericPair& numericPair) : NumericPair(numericPair) {}
		Range(float min, float max) : NumericPair(min, max) {}

		float getMin() const { return getN1(); }
		void setMin(float min) { setN1(min); }
		float getMax() const { return getN2(); }
		void setMax(float max) { setN2(max); }
	};
}
#endif