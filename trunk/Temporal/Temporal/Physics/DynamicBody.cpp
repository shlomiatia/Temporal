#include "DynamicBody.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Grid.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	const float DynamicBody::GRAVITY(5000.0f);

	DynamicBody::DynamicBody(const Size& size)
		: _size(size), _velocity(Vector::Zero), _force(Vector::Zero), _impulse(Vector::Zero), _gravityEnabled(true), _collision(Vector::Zero)
	{
		assert(_size.getWidth() > 0.0f);
		assert(_size.getHeight() > 0.0f);
	}

	Rect DynamicBody::getBounds(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		return Rect(position, _size);
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
			Rect* outParam = (Rect*)message.getParam();
			*outParam = getBounds();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Rect bounds = getBounds();
			Graphics::get().drawRect(bounds);
		}
		else if(message.getID() == MessageID::SET_FORCE)
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
			_force = Vector::Zero;
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
		determineVelocity(framePeriodInMillis);
		applyVelocity();
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

	void DynamicBody::applyVelocity(void)
	{
		_collision = Vector::Zero;

		// BRODER
		float maxHorizontalStepSize = _size.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = _size.getHeight() / 2.0f - 1.0;
		const float MAX_STEP_SIZE = std::min(maxHorizontalStepSize, maxVerticalStepSize);
		Vector remainingVlocity = _velocity;
		while(remainingVlocity != Vector::Zero)
		{
			Vector stepVelocity = Vector::Zero;
			float movement = remainingVlocity.getLength();
			if(movement <= MAX_STEP_SIZE || _impulse != Vector::Zero)
			{
				stepVelocity = remainingVlocity;
			}
			else
			{
				float ratio = MAX_STEP_SIZE / movement;
				stepVelocity.setVx(remainingVlocity.getVx() * ratio);
				stepVelocity.setVy(remainingVlocity.getVy() * ratio);
			}
			
			remainingVlocity -= stepVelocity;
			changePosition(stepVelocity);

			Rect bounds = getBounds();
			Grid::get().iterateTiles(bounds, this, NULL, correctCollision);
			if(_collision != Vector::Zero)
				break;

		}
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
		_impulse = Vector::Zero;
	}

	bool DynamicBody::correctCollision(const StaticBody& staticBody)
	{
		const Segment& staticBodyBounds(staticBody.getSegment());
		const Rect& dynamicBodyBounds(getBounds());
		Vector correction = Vector::Zero;
		if(!staticBody.isCover() && dynamicBodyBounds.intersects(staticBodyBounds, &correction))
		{
			for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
			{
				if(correction.getAxis(axis) * _velocity.getAxis(axis) < 0.0f)
				{
					_force.setAxis(axis, 0.0f);
				}
			}
			if(correction.getVy() > 0.0f)
				_force.setVx(0.0f);
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