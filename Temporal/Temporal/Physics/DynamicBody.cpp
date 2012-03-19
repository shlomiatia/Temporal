#include "DynamicBody.h"
#include "StaticBody.h"
#include "Grid.h"
#include "PhysicsUtils.h"
#include <Temporal\Game\Message.h>
#include <algorithm>

namespace Temporal
{
	const float DynamicBody::GRAVITY(5000.0f);

	float correctCollisionInAxis(float force, float minDynamic, float maxDynamic, float minStatic, float maxStatic)
	{
		if(force > 0 && maxDynamic <= minStatic)
			force = std::min(force, minStatic - maxDynamic);
		else if(force < 0 && minDynamic >= maxStatic)
			force = std::max(force, maxStatic - minDynamic);
		return force;
	}

	Orientation::Enum DynamicBody::getOrientation(void) const
	{
		Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return orientation;
	}

	void DynamicBody::applyVelocity(void) const
	{
		const Vector& position = *(Vector*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Vector newPosition = position + _velocity;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
	}

	void DynamicBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		if(message.getID() == MessageID::SET_FORCE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_force = Vector(param.getX() * getOrientation(), param.getY());
		}
		else if(message.getID() == MessageID::SET_IMPULSE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_impulse = Vector(param.getX() * getOrientation(), param.getY());
			_force = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = *(bool*)message.getParam();
		}
		else if(message.getID() == MessageID::GET_GRAVITY)
		{
			message.setParam((void*)&GRAVITY);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();
			update(framePeriodInMillis);
		}
	}

	void DynamicBody::update(float framePeriodInMillis)
	{
		determineVelocity(framePeriodInMillis);
		handleCollisions();
	}

	void DynamicBody::correctCollision(const StaticBody& staticBody)
	{
		const Rect& staticBodyBounds(staticBody.getBounds());
		const Rect& dynamicBodyBounds(getBounds());
		const Rect& futureBounds = dynamicBodyBounds + _velocity;

		if(!staticBody.isCover() && futureBounds.intersectsExclusive(staticBodyBounds))
		{
			// TODO: Correct smallest axis PHYSICS
			// TODO: Gradual test PHYSICS
			float x = correctCollisionInAxis(_velocity.getX(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight(), staticBodyBounds.getLeft(), staticBodyBounds.getRight());
			float y = correctCollisionInAxis(_velocity.getY(), dynamicBodyBounds.getBottom(), dynamicBodyBounds.getTop(), staticBodyBounds.getBottom(), staticBodyBounds.getTop());
			_velocity.setX(x);
			_velocity.setY(y);
		}
	}

	void DynamicBody::detectCollision(const StaticBody& staticBody)
	{
		if(!staticBody.isCover())
		{
			const Rect& dynamicBodyBounds = getBounds();
			Orientation::Enum dynamicBodyOrientation = getOrientation();
			const Rect& staticBodyBounds = staticBody.getBounds();
			Direction::Enum collision = calculateCollision(dynamicBodyBounds, dynamicBodyOrientation, staticBodyBounds);
			_collision = _collision | collision;
		}
	}

	bool DynamicBody::correctCollision(void* caller, void* data, const StaticBody& staticBody)
	{
		DynamicBody* dynamicBody = (DynamicBody*)caller;
		dynamicBody->correctCollision(staticBody);
		return true;
	}

	bool DynamicBody::detectCollision(void* caller, void* data, const StaticBody& staticBody)
	{
		DynamicBody* dynamicBody = (DynamicBody*)caller;
		dynamicBody->detectCollision(staticBody);
		return true;
	}

	void DynamicBody::determineVelocity(float framePeriodInMillis)
	{
		float interpolation = framePeriodInMillis / 1000.0f;
		_velocity = Vector::Zero;

		if(_impulse == Vector::Zero)
		{
			_velocity = _force * interpolation;
		}
		else
		{
			_velocity = _impulse;
			_impulse = Vector::Zero;
		}
		if(_gravityEnabled)
		{
			float gravity = 0.5f * GRAVITY * pow(interpolation, 2.0f);
			float y = _velocity.getY();
			y -= gravity;
			_velocity.setY(y);
			_force -= Vector(0.0f, GRAVITY * interpolation);
		}	
	}

	void DynamicBody::handleCollisions(void)
	{
		Rect bounds = getBounds() + _velocity;
		Grid::get().iterateTiles(bounds, this, NULL, correctCollision);

		applyVelocity();

		bounds = getBounds();
		_collision = Direction::NONE;
		Grid::get().iterateTiles(bounds, this, NULL, detectCollision);

		// TODO: Broder help me!!! SLOTH!
		if(_velocity.getY() == 0.0f)
			_force.setY(0.0f);
		if(_collision & Direction::BOTTOM ||  _velocity.getX() == 0.0f)
			_force.setX(0.0f);
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
	}

}