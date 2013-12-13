#include "Fixture.h"
#include "Vector.h"
#include "Transform.h"
#include "CollisionFilter.h"

namespace Temporal
{
	void Fixture::init(const Component& parent)
	{
		_transform = static_cast<const Transform*>(parent.getEntity().get(Transform::TYPE));
		_filter = static_cast<const CollisionFilter*>(parent.getEntity().get(CollisionFilter::TYPE));
		_globalShape = _localShape;
		update();
	}

	void Fixture::update()
	{
		Vector center = _localShape.getCenter();
		center.setX(center.getX() * _transform->getOrientation());
		center += _transform->getPosition();
		_globalShape.setCenter(center);
	}

	Hash Fixture::getEntityId() const
	{
		return _transform->getEntity().getId();
	}

	Fixture* Fixture::clone() const
	{
		return new Fixture(_localShape);
	}
}