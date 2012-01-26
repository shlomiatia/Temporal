#include "Body.h"
#include "Physics.h"
#include <algorithm>
#include <assert.h>

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

	Body::Body(bool isDynamic, const Vector& position, const Vector& size, Orientation::Type orientation)
		: _isDynamic(isDynamic), _bounds(position, size), _orientation(orientation), _force(Vector::Zero), _gravityEnabled(true), _elementsCount(0), _collision(Direction::NONE)
	{
		assert(getBounds().getSize().getX() > 0);
		assert(getBounds().getSize().getY() > 0);
	}

	Body::~Body(void)
	{
		for(int i = 0; i < _elementsCount; ++i) 
		{
			delete _elements[i]; 
		} 
	}

	bool Body::rayCast(const Body& other) const
	{
		return Physics::get().rayCast(*this, other);
	}

	void Body::applyGravity(void)
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

	void Body::correctCollision(const Rect& bodyBounds)
	{
		const Rect& futureBounds = getBounds() + getForce();
		
		if(futureBounds.intersectsExclusive(bodyBounds))
		{
			float x = correctCollisionInAxis(getForce().getX(), getBounds().getLeft(), getBounds().getRight(), bodyBounds.getLeft(), bodyBounds.getRight());
			float y = correctCollisionInAxis(getForce().getY(), getBounds().getBottom(), getBounds().getTop(), bodyBounds.getBottom(), bodyBounds.getTop());
			_force.setX(x);
			_force.setY(y);
		}
	}
}