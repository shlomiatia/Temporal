#include "CollisionInfo.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "Transform.h"

namespace Temporal
{
	CollisionInfo::CollisionInfo(const Transform& transform, const CollisionFilter& filter, const Shape* shape)
			: _transform(transform), _filter(filter), _localShape(shape), _globalShape(shape->clone()) {}

	const Shape& CollisionInfo::getGlobalShape()
	{
		_globalShape->setCenter(_localShape->getCenter());
		_globalShape->rotate(_transform.getOrientation());
		_globalShape->translate(_transform.getPosition());
		return *_globalShape;
	}

}