#include "Sensor.h"

namespace Temporal
{
	Sensor::Sensor(SensorID::Type id, DynamicBody& owner, const Vector& offsetFromBody, const Vector& size, Direction::Type positive, Direction::Type negative)
			: _id(id), _owner(owner), _offsetFromBody(offsetFromBody), _size(size), _sensedBody(NULL), _positive(positive), _negative(negative) { owner.add(this); }

	Rect Sensor::getBounds(void) const
	{
		const Rect& ownerBounds(_owner.getBounds());
		return Rect(ownerBounds.getCenterX() + _offsetFromBody.getX() * _owner.getOrientation(),
					ownerBounds.getCenterY() + _offsetFromBody.getY(),
					_size.getWidth(),
					_size.getHeight());
	}
}