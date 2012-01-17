#include "Body.h"
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

	Body::~Body(void)
	{
		for(int i = 0; i < _elementsCount; ++i) 
		{
			delete _elements[i]; 
		} 
	}

	void Body::applyGravity(float gravity, float maxGravity)
	{
		float y = getForce().getY();
		y -= gravity * getGravityScale();
		if(y < maxGravity)
			y = maxGravity;
		_force.setY(y);
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