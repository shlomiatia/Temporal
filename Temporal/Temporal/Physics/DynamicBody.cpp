#include "DynamicBody.h"
#include "Physics.h"
#include <algorithm>

namespace Temporal
{
	float correctCollisionInAxis(float force, float minDynamic, float maxDynamic, float minStatic, float maxStatic)
	{
		if(force > 0 && maxDynamic <= minStatic)
			force = std::min(force, minStatic - maxDynamic);
		else if(force < 0 && minDynamic >= maxStatic)
			force = std::max(force, maxStatic - minDynamic);
		return force;
	}

	DynamicBody::DynamicBody(const Vector& position, const Vector& size, Orientation::Type orientation)
		: Body(position, size), _orientation(orientation), _force(Vector::Zero), _gravityEnabled(true), _elementsCount(0), _collision(Direction::NONE)
	{}

	bool DynamicBody::rayCast(const DynamicBody& other) const
	{
		return Physics::get().rayCast(*this, other);
	}

	void DynamicBody::applyGravity(void)
	{
		if(isGravityEnabled())
		{
			float y = getForce().getY();
			y -= Physics::GRAVITY;
			if(y < Physics::MAX_GRAVITY)
				y = Physics::MAX_GRAVITY;
			_force.setY(y);
		}
	}

	void DynamicBody::correctCollision(const Rect& DynamicBodyBounds)
	{
		const Rect& futureBounds = getBounds() + getForce();
		
		if(futureBounds.intersectsExclusive(DynamicBodyBounds))
		{
			float x = correctCollisionInAxis(getForce().getX(), getBounds().getLeft(), getBounds().getRight(), DynamicBodyBounds.getLeft(), DynamicBodyBounds.getRight());
			float y = correctCollisionInAxis(getForce().getY(), getBounds().getBottom(), getBounds().getTop(), DynamicBodyBounds.getBottom(), DynamicBodyBounds.getTop());
			_force.setX(x);
			_force.setY(y);
		}
	}
}