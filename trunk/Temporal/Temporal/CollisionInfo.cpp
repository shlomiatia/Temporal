#include "CollisionInfo.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "Transform.h"

namespace Temporal
{
	CollisionInfo::CollisionInfo(const Transform& transform, const Shape* shape, int filter1, int mask1, int filter2, int mask2)
			: _transform(transform), _localShape(shape), _globalShape(shape->clone()), _filter1(filter1), _mask1(mask1), _filter2(filter2), _mask2(mask2) {}

	const Shape& CollisionInfo::getGlobalShape()
	{
		_globalShape->setCenter(_localShape->getCenter());
		_globalShape->rotate(_transform.getOrientation());
		_globalShape->translate(_transform.getPosition());
		return *_globalShape;
	}

	bool CollisionInfo::canCollide(const CollisionInfo& other) const
	{
		return (_mask1 & other.getFilter1()) != 0 &&
			   (other.getMask1() & _filter1) != 0 &&
			   (_mask2 & other.getFilter2()) != 0 &&
			   (other.getMask2() & _filter2) != 0;
	}
}