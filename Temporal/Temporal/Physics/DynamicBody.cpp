#include "DynamicBody.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Grid.h"
#include <Temporal\Base\ShapeOperations.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	const Vector DynamicBody::GRAVITY(0.0f, -4500.0f);

	DynamicBody::DynamicBody(const Size& size)
		: _size(size), _velocity(Vector::Zero), _absoluteImpulse(Vector::Zero), _gravityEnabled(true), _collision(Vector::Zero)
	{
		assert(_size.getWidth() > 0.0f);
		assert(_size.getHeight() > 0.0f);
	}

	Rectangle DynamicBody::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		return Rectangle(position, _size);
	}

	Orientation::Enum DynamicBody::getOrientation(void) const
	{
		Orientation::Enum orientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return orientation;
	}

	void DynamicBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SIZE)
		{
			message.setParam(&_size);
		}
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			Rectangle* outParam = (Rectangle*)message.getParam();
			*outParam = getBounds();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Rectangle bounds = getBounds();
			Graphics::get().draw(bounds);
		}
		else if(message.getID() == MessageID::SET_TIME_BASED_IMPULSE)
		{
			const Vector& param = *(Vector*)message.getParam();
			Vector timeBasedImpulse = Vector(param.getVx() * getOrientation(), param.getVy());
			if(timeBasedImpulse == Vector::Zero)
				_velocity = Vector::Zero;
			else
				_velocity += timeBasedImpulse;
		}
		else if(message.getID() == MessageID::SET_ABSOLUTE_IMPULSE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_absoluteImpulse = Vector(param.getVx() * getOrientation(), param.getVy());
			//_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = *(bool*)message.getParam();
			//_velocity = Vector::Zero;
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

	void DynamicBody::changePosition(const Vector& offset)
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Point newPosition = position + offset;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
	}

	void DynamicBody::update(float framePeriodInMillis)
	{
		float interpolation = framePeriodInMillis / 1000.0f;
		Vector movement = _velocity * interpolation + _absoluteImpulse;
		if(_gravityEnabled)
		{
			movement += (GRAVITY * pow(interpolation, 2.0f)) / 2.0f;
			_velocity += GRAVITY * interpolation;
		}
		float maxHorizontalStepSize = _size.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = _size.getHeight() / 2.0f - 1.0f;
		const float MAX_STEP_SIZE = std::min(maxHorizontalStepSize, maxVerticalStepSize);
		_collision = Vector::Zero;
		while(movement != Vector::Zero)
		{
			Vector stepMovement = Vector::Zero;
			float movementAmount = movement.getLength();
			if(movementAmount <= MAX_STEP_SIZE || _absoluteImpulse != Vector::Zero)
			{
				stepMovement = movement;
			}
			else
			{
				float ratio = MAX_STEP_SIZE / movementAmount;
				stepMovement.setVx(movement.getVx() * ratio);
				stepMovement.setVy(movement.getVy() * ratio);
			}
			
			movement -= stepMovement;
			changePosition(stepMovement);

			Rectangle bounds = getBounds();
			Grid::get().iterateTiles(bounds, this, NULL, correctCollision);
			if(_collision != Vector::Zero)
				break;
		}
		
		if(_collision.getVy() == 0.0f)
			int i = 0;
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
		_absoluteImpulse = Vector::Zero;
	}

	bool DynamicBody::correctCollision(const StaticBody& staticBody)
	{
		const Shape& staticBodyBounds = staticBody.getShape();
		const Rectangle& dynamicBodyBounds = getBounds();
		Vector correction = Vector::Zero;
		if(!staticBody.isCover() && intersects(dynamicBodyBounds, staticBodyBounds, &correction))
		{
			for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
			{
				if(correction.getAxis(axis) * _velocity.getAxis(axis) < 0.0f)
				{
					_velocity.setAxis(axis, 0.0f);
				}
			}
			changePosition(correction);
			_collision -= correction;
		}
		return true;
	}

	bool DynamicBody::correctCollision(void* caller, void* data, const StaticBody& staticBody)
	{
		DynamicBody* dynamicBody = (DynamicBody*)caller;
		return dynamicBody->correctCollision(staticBody);
	}
}