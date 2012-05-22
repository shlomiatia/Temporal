#ifndef DIRECTEDINTERVAL_H
#define DIRECTEDINTERVAL_H
#include "NumericPair.h"
#include "Math.h"

namespace Temporal
{
	class DirectedInterval
	{
	public:
		virtual ~DirectedInterval(void) {};
		virtual Vector getOrigin(void) const = 0;
		virtual Vector getDirection(void) const = 0;
		virtual Point getTarget(void) const = 0;
		virtual float getLength(void) const = 0;
		Vector getVector(void) const { return getTarget() - getOrigin(); }
	};

	class Ray : public DirectedInterval
	{
	public:
		Ray(const Point& origin, const Vector& direction) : _origin(origin), _direction(direction) {}

		Vector getOrigin(void) const { return _origin; }
		Vector getDirection(void) const { return _direction; }
		Point getTarget(void) const { return Point(_direction.getVx() >= 0.0f ? FLOAT_MAX : -FLOAT_MAX, _direction.getVy() >= 0.0f ? FLOAT_MAX : -FLOAT_MAX); }
		float getLength(void) const { return FLOAT_MAX; }

	private:
		Point _origin;
		Vector _direction;
	};

	class DirectedSegment : public DirectedInterval
	{
	public:
		DirectedSegment(float x1, float y1, float x2, float y2) : _origin(x1, y1), _vector(x2 - x1, y2 - y1) {}
		DirectedSegment(const Point& origin, const Vector& vector) : _origin(origin), _vector(vector) {}
		Vector getOrigin(void) const { return _origin; }
		Vector getDirection(void) const { return _vector.normalize(); }
		Point getTarget(void) const { return getOrigin() + _vector; }
		float getLength(void) const { return _vector.getLength(); }
	private:
		Point _origin;
		Vector _vector;
	};
}
#endif