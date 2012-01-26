#include "Sensor.h"

namespace Temporal
{
	Sensor::Sensor(DynamicBody& owner, const Vector& offsetFromBody, const Vector& size, Direction::Type positive, Direction::Type negative)
			: _owner(owner), _offsetFromBody(offsetFromBody), _size(size), _sensedBody(NULL), _positive(positive), _negative(negative) { owner.add(this); }

	Rect Sensor::getBounds(void) const
	{
		return Rect(_owner.getPosition().getX() + _offsetFromBody.getX() * _owner.getOrientation(),
					_owner.getPosition().getY() + _offsetFromBody.getY(),
					_size.getWidth(),
					_size.getHeight());
	}
}