#ifndef AABB_H
#define AABB_H
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Shape.h"

namespace Temporal
{
	class AABB : public Shape
	{
	public:
		static const AABB Empty;

		AABB(float centerX, float centerY, float width, float height);
		AABB(const Point& center, const Size& size);
		AABB(const Point& center, const Vector& radius);

		ShapeType::Enum getType(void) const { return ShapeType::AABB; }

		const Point& getCenter(void) const { return _center; }
		const Vector& getRadius(void) const { return _radius; }

		float getCenterX(void) const { return getCenter().getX(); }
		float getCenterY(void) const { return getCenter().getY(); }

		float getRadiusVx(void) const { return getRadius().getVx(); }
		float getRadiusVy(void) const { return getRadius().getVy(); }

		float getBottom(void) const { return getCenterY() - getRadiusVy(); }
		float getLeft(void) const {	return getCenterX() - getRadiusVx(); }
		float getTop(void) const { return getCenterY() + getRadiusVy(); }
		float getRight(void) const { return getCenterX() + getRadiusVx(); }

		float getWidth(void) const { return getRadiusVx() * 2.0f; }
		float getHeight(void) const { return getRadiusVy() * 2.0f; }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		bool contains(const Point& point) const;
	private:
		Point _center;
		Vector _radius;

		void validate(void) const;
	};

	inline AABB AABBLB(float left, float bottom, float width, float height) { return AABB(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline AABB AABBCB(float centerX, float bottom, float width, float height) { return AABB(centerX, bottom + height / 2.0f, width, height); }
	inline AABB AABBLC(float left, float centerY, float width, float height) { return AABB(left + width / 2.0f, centerY, width, height); }
}
#endif