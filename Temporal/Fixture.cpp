#include "Fixture.h"
#include "Vector.h"
#include "Transform.h"
#include "CollisionFilter.h"

namespace Temporal
{
	void Fixture::init(const Component& parent)
	{
		_transform = static_cast<const Transform*>(parent.getEntity().get(ComponentsIds::TRANSFORM));
		_filter = static_cast<const CollisionFilter*>(parent.getEntity().get(ComponentsIds::COLLISION_FILTER));
		_globalShape = _localShape;
		update();
	}

	void Fixture::update()
	{
		if(_localShape.getAngle() != _globalShape.getAngle() || _localShape.getRadius() != _globalShape.getRadius())
		{
			_globalShape = _localShape;
		}
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

	int Fixture::getCategory() const
	{
		if (_filter)
			return _filter->getCategory();
		else
			return -1;
	}

	int Fixture::getGroup() const
	{
		if (_filter)
			return _filter->getGroup();
		else
			return -1;
	}

	bool Fixture::canCollide(int mask, int group) const
	{
		return 
			_isEnabled &&
			(getCategory() != -1 && (mask & getCategory())) != 0 &&
			(getGroup() == -1 ||
			group == -1 ||
			getGroup() == group);
	}
}