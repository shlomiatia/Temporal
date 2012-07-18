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
		static const AABB Zero;

		AABB(float centerX, float centerY, float width, float height);
		AABB(const Point& center, const Size& size);
		AABB(const Point& center, const Vector& radius);

		ShapeType::Enum getType() const { return ShapeType::AABB; }

		const Point& getCenter() const { return _center; }
		const Vector& getRadius() const { return _radius; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		float getRadiusVx() const { return getRadius().getVx(); }
		float getRadiusVy() const { return getRadius().getVy(); }

		float getBottom() const { return getCenterY() - getRadiusVy(); }
		float getLeft() const {	return getCenterX() - getRadiusVx(); }
		float getTop() const { return getCenterY() + getRadiusVy(); }
		float getRight() const { return getCenterX() + getRadiusVx(); }

		float getWidth() const { return getRadiusVx() * 2.0f; }
		float getHeight() const { return getRadiusVy() * 2.0f; }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		bool contains(const Point& point) const;

		bool operator==(const AABB& other) const { return ((getCenter() == other.getCenter()) && (getRadius() == other.getRadius())); }
		bool operator!=(const AABB& other) const { return !(*this == other); }
	private:
		Point _center;
		Vector _radius;

		void validate() const;
	};

	inline AABB AABBLB(float left, float bottom, float width, float height) { return AABB(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline AABB AABBCB(float centerX, float bottom, float width, float height) { return AABB(centerX, bottom + height / 2.0f, width, height); }
	inline AABB AABBLC(float left, float centerY, float width, float height) { return AABB(left + width / 2.0f, centerY, width, height); }
}
#endif