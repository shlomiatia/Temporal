#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Shape.h"
#include <stdio.h>

namespace Temporal
{
	class Rectangle : public Shape
	{
	public:
		static const Rectangle Empty;

		Rectangle(float centerX, float centerY, float width, float height);
		Rectangle(const Point& center, const Size& size);
		Rectangle(const Point& center, const Vector& radius);

		ShapeType::Enum getType(void) const { return ShapeType::RECTANGLE; }

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

		Rectangle move(const Vector& vector) const { return Rectangle(getCenter() + vector, getRadius()); }
		Rectangle resize(const Vector& delta) const { return Rectangle(getCenter(), Vector(getRadius() + delta / 2.0f)); }
		
		bool contains(const Point& point) const;
	private:
		Point _center;
		Vector _radius;

		void validate(void) const;
	};

	inline Rectangle RectangleLB(float left, float bottom, float width, float height) { return Rectangle(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline Rectangle RectangleCB(float centerX, float bottom, float width, float height) { return Rectangle(centerX, bottom + height / 2.0f, width, height); }
	inline Rectangle RectangleLC(float left, float centerY, float width, float height) { return Rectangle(left + width / 2.0f, centerY, width, height); }
}
#endif