#ifndef SLOPEDAREA_H
#define SLOPEDAREA_H
#include "NumericPair.h"
#include <math.h>

namespace Temporal
{
	// A parallelogram with 1 axis parallel to Y
	class YABP
	{
	public:
		YABP(const Point& center, const Vector& slopedRadius, float yRadius) : _center(center), _slopedRadius(slopedRadius), _yRadius(yRadius) {}

		const Point& getCenter(void) const { return _center; }
		const Vector& getSlopedRadius(void) const { return _slopedRadius; }
		float getYRadius(void) const { return _yRadius; }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }
		
		float getSlopedRadiusVx(void) const { return getSlopedRadius().getVx(); }
		float getSlopedRadiusVy(void) const { return getSlopedRadius().getVy(); }

		float getLeft(void) const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight(void) const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop(void) const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
		float getBottom(void) const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }

		float getWidth(void) const { return getSlopedRadius().getLength() * 2.0f; }
		float getHeight(void) const { return getYRadius() * 2.0f; }

		YABP resize(const Vector& delta) const { return YABP(getCenter(), getSlopedRadius().normalize() * (getSlopedRadius().getLength() + delta.getVx() / 2.0f), getYRadius() + delta.getVy() / 2.0f); }
	private:
		Point _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate(void) const;
	};
}
#endif