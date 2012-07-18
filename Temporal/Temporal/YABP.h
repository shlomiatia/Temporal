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

		const Point& getCenter() const { return _center; }
		const Vector& getSlopedRadius() const { return _slopedRadius; }
		float getYRadius() const { return _yRadius; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		Vector getYVector() const { return Vector(0.0f, getYRadius()); }
		
		float getSlopedRadiusVx() const { return getSlopedRadius().getVx(); }
		float getSlopedRadiusVy() const { return getSlopedRadius().getVy(); }

		ShapeType::Enum getType() const { return ShapeType::YABP; }

		float getLeft() const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight() const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop() const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
		float getBottom() const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }

	private:
		Point _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate() const;
	};
}
#endif