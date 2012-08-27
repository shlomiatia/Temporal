#include "Fixture.h"
#include "Vector.h"
#include "Transform.h"
#include "CollisionFilter.h"

namespace Temporal
{
	Fixture::Fixture(const Shape* shape)
			: _localShape(shape), _globalShape(shape->clone())
	{
	}

	void Fixture::init(const Component& parent)
	{
		_transform = static_cast<const Transform*>(parent.getEntity().get(ComponentType::TRANSFORM));
		_filter = static_cast<const CollisionFilter*>(parent.getEntity().get(ComponentType::COLLISION_FILTER));
		update();
	}

	void Fixture::update()
	{
		_globalShape->setCenter(_localShape->getCenter());
		_globalShape->rotate(_transform->getOrientation());
		_globalShape->translate(_transform->getPosition());
	}

	const Hash& Fixture::getEntityId() const
	{
		return _transform->getEntity().getId();
	}
}