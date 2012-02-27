#include "DynamicBody.h"
#include "StaticBody.h"
#include "Utils.h"
#include <Temporal/Game/MessageParams.h>
#include <Temporal/Game/QueryManager.h>
#include <Temporal/Graphics/Graphics.h>
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
		Orientation::Enum orientation = *(const Orientation::Enum* const)sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return orientation;
	}

	void DynamicBody::applyMovement(const Vector& movement) const
	{
		const Vector& position = *(const Vector* const)sendQueryMessageToOwner(Message(MessageID::GET_POSITION));
		Vector newPosition = position + movement;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
	}

	void DynamicBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		if(message.getID() == MessageID::SET_FORCE)
		{
			const Vector& param = *(const Vector* const)message.getParam();
			_movement = Vector(param.getX() * getOrientation(), param.getY());
		}
		else if(message.getID() == MessageID::SET_IMPULSE)
		{
			const Vector& param = *(const Vector* const)message.getParam();
			_movement = Vector(param.getX() * getOrientation(), param.getY());
			_isImpulse = true;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = *(const bool* const)message.getParam();
		}
		else if(message.getID() == MessageID::GET_GRAVITY)
		{
			message.setParam(&GRAVITY);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(const float* const)message.getParam();
			update(framePeriodInMillis);
		}
	}

	void DynamicBody::update(float framePeriodInMillis)
	{
		ComponentOfTypeIteraor iterator = QueryManager::get().getComponentOfTypeIteraor(ComponentType::STATIC_BODY);
		float interpolation = framePeriodInMillis / 1000.0f;

		Vector velocity(Vector::Zero);
		if(!_isImpulse)
		{
			velocity = _movement * interpolation;
		}
		else
		{
			velocity = _movement;
			_isImpulse = false;
			_movement = Vector::Zero;
		}
		if(_gravityEnabled)
		{
			float gravity = 0.5f * GRAVITY * pow(interpolation, 2.0f);
			float y = velocity.getY();
			y -= gravity;
			velocity.setY(y);
		}
		while(iterator.next())
		{
			StaticBody& staticBody = (StaticBody&)iterator.current();
			correctCollision(staticBody, velocity);
		}
		applyMovement(velocity);
		if(_gravityEnabled)
			_movement -= Vector(0.0f, GRAVITY * interpolation);

		_collision = Direction::NONE;
		iterator.reset();
		while(iterator.next())
		{
			StaticBody& staticBody = (StaticBody&)iterator.current();
			detectCollision(staticBody);
		}
		if(_collision & Direction::BOTTOM)
		{
			_movement = Vector::Zero;
		}
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
	}

	void DynamicBody::correctCollision(const StaticBody& staticBody, Vector& velocity)
	{
		const Rect& staticBodyBounds(staticBody.getBounds());
		const Rect& dynamicBodyBounds(getBounds());
		const Rect& futureBounds = dynamicBodyBounds + velocity;

		if(!staticBody.isCover() && futureBounds.intersectsExclusive(staticBodyBounds))
		{
			// TODO: Correct smallest axis
			// TODO: Gradual test
			float x = correctCollisionInAxis(velocity.getX(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight(), staticBodyBounds.getLeft(), staticBodyBounds.getRight());
			float y = correctCollisionInAxis(velocity.getY(), dynamicBodyBounds.getBottom(), dynamicBodyBounds.getTop(), staticBodyBounds.getBottom(), staticBodyBounds.getTop());
			velocity.setX(x);
			velocity.setY(y);
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
}