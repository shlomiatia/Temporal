#ifndef YABP_H
#define YABP_H
#include "NumericPair.h"
#include "Shape.h"
#include <math.h>

namespace Temporal
{
	// A parallelogram with 1 axis parallel to Y
	class YABP : public Shape
	{
	public:
		YABP(const Point& center, const Vector& slopedRadius, float yRadius);

		const Point& getCenter(void) const { return _center; }
		const Vector& getSlopedRadius(void) const { return _slopedRadius; }
		float getYRadius(void) const { return _yRadius; }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }

		Vector getYVector(void) const { return Vector(0.0f, getYRadius()); }
		
		float getSlopedRadiusVx(void) const { return getSlopedRadius().getVx(); }
		float getSlopedRadiusVy(void) const { return getSlopedRadius().getVy(); }

		ShapeType::Enum getType(void) const { return ShapeType::YABP; }

		float getLeft(void) const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight(void) const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop(void) const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
		float getBottom(void) const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }

	private:
		Point _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate(void) const;
	};
}
#endif