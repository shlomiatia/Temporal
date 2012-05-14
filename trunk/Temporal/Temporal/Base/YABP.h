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

		Vector getYVector(void) const { return Vector(0.0f, getYRadius()); }
		
		float getSlopedRadiusVx(void) const { return getSlopedRadius().getVx(); }
		float getSlopedRadiusVy(void) const { return getSlopedRadius().getVy(); }

		float getLeft(void) const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight(void) const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop(void) const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
		float getBottom(void) const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }

		float getWidth(void) const { return getSlopedRadiusVx() * 2.0f; }
		float getHeight(void) const { return getYRadius() * 2.0f + abs(getSlopedRadiusVy()) * 2.0f; }
	private:
		Point _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate(void) const;
	};
}
#endif