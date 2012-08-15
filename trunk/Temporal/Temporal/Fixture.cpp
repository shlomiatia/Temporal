#include "Fixture.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "Transform.h"

namespace Temporal
{
	Fixture::Fixture(const Transform& transform, const CollisionFilter& filter, const Shape* shape)
			: _transform(transform), _filter(filter), _localShape(shape), _globalShape(shape->clone())
	{
		update();
	}

	void Fixture::update()
	{
		_globalShape->setCenter(_localShape->getCenter());
		_globalShape->rotate(_transform.getOrientation());
		_globalShape->translate(_transform.getPosition());
	}

	const Hash& Fixture::getEntityId() const
	{
		return _transform.getEntity().getId();
	}
}