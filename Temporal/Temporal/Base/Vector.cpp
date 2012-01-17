#include "Vector.h"

namespace Temporal
{
	const Vector Vector::Zero(0.0f, 0.0f);

	Vector operator+(const Vector& vector, float scalar) { return Vector(vector.getX() + scalar, vector.getY() + scalar); }
	Vector operator-(const Vector& vector, float scalar) { return vector + -scalar; }
	Vector operator*(const Vector& vector, float scalar) { return Vector(vector.getX() * scalar, vector.getY() * scalar); }
	Vector operator/(const Vector& vector, float scalar) { return vector * (1.0f / scalar); }
}