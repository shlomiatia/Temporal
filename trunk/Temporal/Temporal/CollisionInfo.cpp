#include "CollisionInfo.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "Transform.h"

namespace Temporal
{
	CollisionInfo::CollisionInfo(const Transform& transform, const Shape* shape, int filter)
			: _transform(transform), _localShape(shape), _filter(filter), _globalShape(shape->clone()) {}

	const Shape& CollisionInfo::getGlobalShape() const
	{
		_globalShape->setCenter(_localShape->getCenter());
		_globalShape->rotate(_transform.getOrientation());
		_globalShape->translate(_transform.getPosition());
		return *_globalShape;
	}
}