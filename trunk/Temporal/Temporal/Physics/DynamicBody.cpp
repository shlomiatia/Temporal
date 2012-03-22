#include "DynamicBody.h"
#include "StaticBody.h"
#include "DynamicBody.h"
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
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Point newPosition = position + _velocity;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
	}

	void DynamicBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		if(message.getID() == MessageID::SET_FORCE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_force = Vector(param.getVx() * getOrientation(), param.getVy());
		}
		else if(message.getID() == MessageID::SET_IMPULSE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_impulse = Vector(param.getVx() * getOrientation(), param.getVy());
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
		const Rect& futureBounds = dynamicBodyBounds.move(_velocity);

		if(!staticBody.isCover() && futureBounds.intersectsExclusive(staticBodyBounds))
		{
			for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
			{
				Range dynamicAxisRange = dynamicBodyBounds.getAxis(axis);
				float dynamicAxisMin = dynamicAxisRange.getMin();
				float dynamicAxisMax = dynamicAxisRange.getMax();
				Range staticAxisRange = staticBodyBounds.getAxis(axis);
				float staticAxisMin = staticAxisRange.getMin();
				float staticAxisMax = staticAxisRange.getMax();
				float axisVelocity = _velocity.getAxis(axis);
				if(axisVelocity > 0 && dynamicAxisMax <= staticAxisMin)
				{
					float corrected = std::min(axisVelocity, staticAxisMin - dynamicAxisMax);
					_velocity.setAxis(axis, corrected);
				}
				else if(axisVelocity < 0 && dynamicAxisMin >= staticAxisMax)
				{
					float corrected = std::max(axisVelocity, staticAxisMax - dynamicAxisMin);
					_velocity.setAxis(axis, corrected);
				}
			}
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
			float y = _velocity.getVy();
			y -= gravity;
			_velocity.setVy(y);
			_force -= Vector(0.0f, GRAVITY * interpolation);
		}	
	}

	void DynamicBody::handleCollisions(void)
	{
		Rect bounds = getBounds().move(_velocity);
		Grid::get().iterateTiles(bounds, this, NULL, correctCollision);

		applyVelocity();

		bounds = getBounds();
		_collision = Direction::NONE;
		Grid::get().iterateTiles(bounds, this, NULL, detectCollision);

		// TODO: Broder help me!!! SLOTH!
		if(_velocity.getVy() == 0.0f)
			_force.setVy(0.0f);
		if(_collision & Direction::BOTTOM ||  _velocity.getVx() == 0.0f)
			_force.setVx(0.0f);
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
	}

}