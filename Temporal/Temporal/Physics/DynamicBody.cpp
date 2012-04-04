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
			if(abs(minYCorrection) < abs(minXCorrection))
			{
				if(minYCorrection != 0.0f)
				{
					correction.setVy(minYCorrection);
					if(minYCorrection * _velocity.getVy() < 0.0f)
						_force.setVy(0.0);
					if(minYCorrection > 0.0f)
					{
						_collision = _collision | Direction::BOTTOM;

						// BRODER
						_force.setVx(0.0f);
					}
					else
					{
						_collision = _collision | Direction::TOP;
					}
				}
			}
			else
			{
				if(minXCorrection != 0.0f)
				{
					correction.setVx(minXCorrection);
					if(minXCorrection * _velocity.getVx() < 0.0f)
						_force.setVx(0.0);
					Orientation::Enum orientation = getOrientation();
					if(minXCorrection > 0.0f)
					{
						_collision = _collision | (orientation == Orientation::LEFT ? Direction::FRONT : Direction::BACK);
					}
					else
					{
						_collision = _collision | (orientation == Orientation::RIGHT ? Direction::FRONT : Direction::BACK);
					}
				}
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
		_collision = Direction::NONE;
		
		Vector remainingVlocity = _velocity;
		while(remainingVlocity != Vector::Zero)
		{
			Rect bounds = getBounds();
			Vector xBasedStepVelocity = Vector::Zero;
			const float MAX_SPEED = 9.0f;
			if(remainingVlocity.getVx() > MAX_SPEED)
			{
				float yRelativeToX = remainingVlocity.getVy() * (MAX_SPEED / remainingVlocity.getVx());
				if(yRelativeToX < MAX_SPEED)
				{
					xBasedStepVelocity.setVx(MAX_SPEED);
					xBasedStepVelocity.setVy(yRelativeToX);
				}
			}
			Vector yBasedStepVelocity = Vector::Zero;
			if(remainingVlocity.getVy() > MAX_SPEED)
			{
				float xRelativeToY = remainingVlocity.getVx() * (MAX_SPEED / remainingVlocity.getVy());
				if(xRelativeToY < MAX_SPEED)
				{
					yBasedStepVelocity.setVy(MAX_SPEED);
					yBasedStepVelocity.setVx(xRelativeToY);
				}
			}
			Vector stepVelocity = xBasedStepVelocity.getLength() > yBasedStepVelocity.getLength() ? xBasedStepVelocity : yBasedStepVelocity;
			if(stepVelocity == Vector::Zero || abs(_velocity.getVy()) == 80.0f)
				stepVelocity = remainingVlocity;
			remainingVlocity -= stepVelocity;
			const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			Point newPosition = position + stepVelocity;
			sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
			bounds = bounds.move(stepVelocity);
			Grid::get().iterateTiles(bounds, this, NULL, correctCollision);
			if((_collision & Direction::BOTTOM) == Direction::NONE)
			{
				int i = 0;
			}
			if(_collision != Direction::NONE)
				break;

		}
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
	}

}