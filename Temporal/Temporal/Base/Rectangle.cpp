#include "Rectangle.h"
#include <assert.h>
#include <algorithm>

namespace Temporal
{
	const Rectangle Rectangle::Empty(Point::Zero, Vector(Vector::Zero));

	Rectangle::Rectangle(float centerX, float centerY, float width, float height)
		: _center(centerX, centerY), _radius(width / 2.0f, height / 2.0f)
	{
		validate();
	}

	Rectangle::Rectangle(const Point& center, const Size& size)
		: _center(center), _radius(size / 2.0f)
	{
		validate();
	}

	Rectangle::Rectangle(const Point& center, const Vector& radius)
		: _center(center), _radius(radius)
	{
		validate();
	}

	void Rectangle::validate(void) const
	{
		assert(getRadiusVx() >= 0);
		assert(getRadiusVy() >= 0);
	}

	bool Rectangle::contains(const Point& point) const
	{
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			if(abs(getCenter().getAxis(axis) - point.getAxis(axis)) > getRadius().getAxis(axis)) return false;
		}
		return true;
	}

	
}