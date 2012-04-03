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
		: _size(size), _velocity(Vector::Zero), _force(Vector::Zero), _impulse(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE)
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

	void DynamicBody::applyVelocity(void) const
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Point newPosition = position + _velocity;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
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
			Graphics::get().drawRect(getBounds());
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

	bool DynamicBody::correctCollision(const StaticBody& staticBody)
	{
		const Segment& staticBodyBounds(staticBody.getSegment());
		const Rect& dynamicBodyBounds(getBounds());
		if(!staticBody.isCover() && dynamicBodyBounds.intersects(staticBodyBounds))
		{
			float upCorrection = staticBodyBounds.getTop() - dynamicBodyBounds.getBottom();
			float downCorrection = staticBodyBounds.getBottom() - dynamicBodyBounds.getTop();
			float minYCorrection = abs(upCorrection) < abs(downCorrection) ? upCorrection : downCorrection;
			float rightCorrection = staticBodyBounds.getRight() - dynamicBodyBounds.getLeft();
			float leftCorrection = staticBodyBounds.getLeft() - dynamicBodyBounds.getRight();
			float minXCorrection = abs(rightCorrection) < abs(leftCorrection) ? rightCorrection : leftCorrection;
			Vector correction(Vector::Zero);

			// When falling, we prefer to stay on the platform we landed on rather then be pushed aside BRODER
			if(abs(minYCorrection) < abs(minXCorrection) || (minYCorrection < 20.0f && minYCorrection > 0.0f))
			{
				correction.setVy(minYCorrection);
				_force.setVy(0.0);
				if(minYCorrection > 0.0f)
				{
					_collision = Direction::BOTTOM;
					_force.setVx(0.0f);
				}
			}
			else
			{
				correction.setVx(minXCorrection);
				_force.setVx(0.0);
			}
			const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			Point newPosition = position + correction;
			sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
			
		}
		return true;
	}

	bool DynamicBody::correctCollision(void* caller, void* data, const StaticBody& staticBody)
	{
		DynamicBody* dynamicBody = (DynamicBody*)caller;
		return dynamicBody->correctCollision(staticBody);
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
		_collision = Direction::NONE;
		applyVelocity();
		Grid::get().iterateTiles(bounds, this, NULL, correctCollision);
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
	}

}