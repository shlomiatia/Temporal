#include "DynamicBody.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Grid.h"
#include <Temporal\Base\Segment.h>
#include <Temporal\Base\ShapeOperations.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	const Vector DynamicBody::GRAVITY(0.0f, -4500.0f);

	float getMaxMovementStepSize(const Size& size)
	{
		float maxHorizontalStepSize = size.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = size.getHeight() / 2.0f - 1.0f;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	DynamicBody::DynamicBody(const Size& size)
		: _size(size), _velocity(Vector::Zero), _absoluteImpulse(Vector::Zero), _gravityEnabled(true), _collision(Vector::Zero), _groundVector(Vector::Zero), MAX_MOVEMENT_STEP_SIZE(getMaxMovementStepSize(size))
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
			{
				_velocity.setVx(0.0f);
				_velocity += timeBasedImpulse;
			}
		}
		else if(message.getID() == MessageID::SET_ABSOLUTE_IMPULSE)
		{
			const Vector& param = *(Vector*)message.getParam();
			_absoluteImpulse = Vector(param.getVx() * getOrientation(), param.getVy());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = *(bool*)message.getParam();
			_velocity = Vector::Zero;
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

		// Only one of this components should be non zero
		Vector movement = _velocity * interpolation + _absoluteImpulse;

		// If moving horizontally on the ground, we adjust to movement according to the ground vector
		if(movement.getVy() == 0.0f && movement.getVx() != 0.0f && _groundVector != Vector::Zero && _absoluteImpulse == Vector::Zero)
		{
			movement = _groundVector * movement.getLength();
		}
		// Apply gravity if needed
		if(_gravityEnabled)
		{
			movement += (GRAVITY * pow(interpolation, 2.0f)) / 2.0f;
			_velocity += GRAVITY * interpolation;
		}
		
		_collision = Vector::Zero;
		_groundVector = Vector::Zero;

		// If the movement is to big, we'll divide it to smaller chuncks
		while(movement != Vector::Zero)
		{
			Vector stepMovement = Vector::Zero;
			float movementAmount = movement.getLength();

			// Absolute impulses are done in one stroke
			if(movementAmount <= MAX_MOVEMENT_STEP_SIZE || _absoluteImpulse != Vector::Zero)
			{
				stepMovement = movement;
			}
			else
			{
				float ratio = MAX_MOVEMENT_STEP_SIZE / movementAmount;
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
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));

		// Absolute impulses last one frame
		_absoluteImpulse = Vector::Zero;
	}

	bool DynamicBody::correctCollision(const StaticBody& staticBody)
	{
		const Shape& staticBodyBounds = staticBody.getShape();
		const Rectangle& dynamicBodyBounds = getBounds();
		Vector correction = Vector::Zero;
		if(!staticBody.isCover() && intersects(dynamicBodyBounds, staticBodyBounds, &correction))
		{
			const Segment& segment = (Segment&)staticBodyBounds;

			// If got collision from below, Calculate ground vector
			if(correction.getVy() >= 0.0f && segment.getPoint1().getX() != segment.getPoint2().getX())
			{
				Vector platformVector = getOrientation() == Orientation::RIGHT ? segment.getPoint2() - segment.getPoint1() : segment.getPoint1() - segment.getPoint2();
				platformVector = platformVector.normalize();
				if(_groundVector == Vector::Zero)
					_groundVector = platformVector;
				else
					_groundVector = (_groundVector + platformVector) / 2.0f;
			}

			// If actor don't want to move horizontally, we allow to correct by y if small enough. This is good to prevent sliding in slopes, and falling from edges
			if(abs(_velocity.getVx()) <= 0.1f && correction.getVx() != 0.0f)
			{	
				float x1 = segment.getPoint1().getX();
				float x2 = segment.getPoint2().getX();
				float y1 = segment.getPoint1().getY();
				float y2 = segment.getPoint2().getY();
				float y = 0.0f;
				// Wall - take top
				if(x1 == x2)
				{
					y = std::max(y1, y2);
				}
				// Floor - take y where the actor stand
				else
				{
					float m =  (y2 - y1) / (x2 - x1);
					float b = y1 - m * x1;
					float x = m > 0 ? dynamicBodyBounds.getRight() : dynamicBodyBounds.getLeft();
					y = m * x + b;
				}
				float yCorrection = y - dynamicBodyBounds.getBottom();

				// BRODER
				correction = Vector(0, yCorrection);
			}
			// Stop the actor where the correction was applied
			for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
			{
				if(correction.getAxis(axis) * _velocity.getAxis(axis) < 0.0f)
				{
					_velocity.setAxis(axis, 0.0f);
				}
			}

			// Do not accumalte velocity if on a floor
			if(correction.getVy() >= 0.0f)
				_velocity.setVx(0.0f);
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