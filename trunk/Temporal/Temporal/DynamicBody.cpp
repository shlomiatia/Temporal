#include "DynamicBody.h"
#include "StaticBody.h"
#include "Grid.h"
#include "BaseUtils.h"
#include "Serialization.h"
#include "Math.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "MessageUtils.h"
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	static const Hash IS_GRAVITY_ENABLED_SERIALIZATION = Hash("DYN_SER_IS_GRAVITY_ENABLED");
	static const NumericPairSerializer VELOCITY_SERIALIZER("DYN_SER_VELOCITY");

	const Vector DynamicBody::GRAVITY(0.0f, -4500.0f);

	float getMaxMovementStepSize(const Size& size)
	{
		float maxHorizontalStepSize = size.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = size.getHeight() / 2.0f - 1.0f;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	DynamicBody::DynamicBody(const Size& size)
		: _size(size), _velocity(Vector::Zero), _absoluteImpulse(Vector::Zero), _gravityEnabled(true), _groundVector(Vector::Zero), MAX_MOVEMENT_STEP_SIZE(getMaxMovementStepSize(size))
	{
		assert(_size.getWidth() > 0.0f);
		assert(_size.getHeight() > 0.0f);
	}

	AABB DynamicBody::getBounds() const
	{
		const Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		return AABB(position, _size);
	}

	Side::Enum DynamicBody::getOrientation() const
	{
		Side::Enum orientation = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		return orientation;
	}

	void DynamicBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SIZE)
		{
			message.setParam(const_cast<Size*>(&_size));
		}
		else if(message.getID() == MessageID::GET_BOUNDS)
		{
			AABB* outParam = static_cast<AABB*>(message.getParam());
			*outParam = getBounds();
		}
		else if(message.getID() == MessageID::GET_GROUND_VECTOR)
		{
			message.setParam(&_groundVector);
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			AABB bounds = getBounds();
			Graphics::get().draw(bounds);
		}
		else if(message.getID() == MessageID::SET_TIME_BASED_IMPULSE)
		{
			const Vector& param = *static_cast<Vector*>(message.getParam());
			Vector timeBasedImpulse = Vector(param.getVx() * getOrientation(), param.getVy());

			// We never want to accumalate horizontal speed from the outside. However, vertical speed need to be accumalted on steep slopes
			_velocity.setVx(0.0f);
			_velocity += timeBasedImpulse;
		}
		else if(message.getID() == MessageID::SET_ABSOLUTE_IMPULSE)
		{
			const Vector& param = *static_cast<Vector*>(message.getParam());
			_absoluteImpulse = Vector(param.getVx() * getOrientation(), param.getVy());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = *static_cast<bool*>(message.getParam());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *static_cast<float*>(message.getParam());
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			serialization.serialize(IS_GRAVITY_ENABLED_SERIALIZATION, _gravityEnabled);
			VELOCITY_SERIALIZER.serialize(serialization, _velocity);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			_gravityEnabled = serialization.deserializeBool(IS_GRAVITY_ENABLED_SERIALIZATION);
			VELOCITY_SERIALIZER.deserialize(serialization, _velocity);
		}
	}

	void DynamicBody::update(float framePeriodInMillis)
	{
		Vector movement = determineMovement(framePeriodInMillis);
		executeMovement(movement);
	}

	Vector DynamicBody::determineMovement(float framePeriodInMillis)
	{
		float interpolation = framePeriodInMillis / 1000.0f;

		// Determine movement
		Vector movement = Vector::Zero;
		if(_absoluteImpulse != Vector::Zero)
		{
			movement = _absoluteImpulse;
		}
		else
		{
			movement = _velocity * interpolation;

			// If moving horizontally on the ground, we adjust to movement according to the ground vector, because we do want no slow downs on moderate slopes
			if(movement.getVy() == 0.0f && movement.getVx() != 0.0f && _groundVector != Vector::Zero)
			{
				movement = (movement.getVx() > 0.0f ? _groundVector : -_groundVector) * movement.getLength();
			}
		}
		// Apply gravity if needed
		if(_gravityEnabled)
		{
			movement += (GRAVITY * pow(interpolation, 2.0f)) / 2.0f;
			_velocity += GRAVITY * interpolation;
		}
		return movement;
	}

	void DynamicBody::executeMovement(Vector movement)
	{
		Vector collision = Vector::Zero;
		_groundVector = Vector::Zero;

		// If the movement is too big, we'll divide it to smaller steps
		while(movement != Vector::Zero)
		{
			Vector stepMovement = Vector::Zero;
			float movementAmount = movement.getLength();

			// Absolute impulses are done in one stroke
			if(movementAmount <= MAX_MOVEMENT_STEP_SIZE || _absoluteImpulse != Vector::Zero)
			{
				stepMovement = movement;
			}
			// Calculate step movement
			else
			{
				float ratio = MAX_MOVEMENT_STEP_SIZE / movementAmount;
				stepMovement.setVx(movement.getVx() * ratio);
				stepMovement.setVy(movement.getVy() * ratio);
			}
			
			movement -= stepMovement;
			changePosition(stepMovement);
			AABB bounds = getBounds();
			
			int collisionFilter = getPeriod(*this);
			Grid::get().iterateTiles(bounds, collisionFilter, this, &collision, detectCollision);
			if(collision != Vector::Zero)
				break;
		}
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &collision));

		// Absolute impulses last one frame
		_absoluteImpulse = Vector::Zero;
	}

	bool DynamicBody::detectCollision(const StaticBody& staticBody, Vector& collision)
	{
		const Shape& staticBodyBounds = staticBody.getShape();
		const AABB& dynamicBodyBounds = getBounds();
		Vector correction = Vector::Zero;
		if(!staticBody.isCover() && intersects(dynamicBodyBounds, staticBodyBounds, &correction))
		{
			correctCollision(dynamicBodyBounds, staticBodyBounds, correction, collision);
		}
		return true;
	}

	void DynamicBody::correctCollision(const AABB& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision)
	{
		const Segment& segment = static_cast<const Segment&>(staticBodyBounds);
		Vector platformVector = segment.getNaturalVector().normalize();
		float angle = platformVector.getAngle();
		bool isModerateSlope = isModerateAngle(angle);

		modifyCorrection(dynamicBodyBounds, segment, correction, isModerateSlope);
		bool isSteepSlope = isSteepAngle(angle);
		modifyVelocity(dynamicBodyBounds, segment, correction, platformVector, isSteepSlope);

		// If got collision from below, calculate ground vector. Only do this for moderate slopes
		if(correction.getVy() >= 0.0f && isModerateSlope)
		{
			if(_groundVector == Vector::Zero || platformVector.getVy() == 0.0f)
				_groundVector = platformVector;
		}

		changePosition(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(const AABB& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope)
	{
		// BRODER
		bool isOnPlatformTopSide =  (dynamicBodyBounds.getLeft() <= segment.getLeft() ||
									 dynamicBodyBounds.getRight() >= segment.getRight()) &&
									 dynamicBodyBounds.getBottom() + 20.0f >= segment.getTop();

		// Prevent x movement on moderate slopes, and on steep slopes/walls when they're on platform edges
		bool canModifyCorrection = isModerateSlope || isOnPlatformTopSide;

		// If actor don't want to move horizontally, we allow to correct by y if small enough. This is good to prevent sliding in slopes, and falling from edges
		if(canModifyCorrection && abs(_velocity.getVx()) < EPSILON && correction.getVx() != 0.0f) 
		{	
			float y = 0.0f;

			// Wall or flat floor - Take top
			if(isOnPlatformTopSide)
			{
				y = segment.getTop();
			}
			// Slopped floor - take y where the actor stand
			else
			{
				float x = segment.getRadiusVy() > 0.0f ? dynamicBodyBounds.getRight() : dynamicBodyBounds.getLeft();
				y = segment.getY(x);
			}
			float yCorrection = y - dynamicBodyBounds.getBottom();

			// BRODER
			if(yCorrection < 20.0f)
				correction = Vector(0, yCorrection);
		}
	}

	void DynamicBody::modifyVelocity(const AABB& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope)
	{
		// Stop the actor where the correction was applied. Also, stop actor horizontal movement if on the floor
		if(differentSign(correction.getVx(), _velocity.getVx()) || correction.getVy() > 0.0f)
			_velocity.setVx(0.0f);
		if(differentSign(correction.getVy(), _velocity.getVy()))
			_velocity.setVy(0.0f);

		// BRODER
		bool isOnPlatform = dynamicBodyBounds.getRight() > segment.getLeft() &&
							dynamicBodyBounds.getLeft() < segment.getRight() &&
							dynamicBodyBounds.getBottom() + 20.0f <= segment.getTop() &&
							dynamicBodyBounds.getBottom() >= segment.getBottom();
		// Slide on steep slopes
		if(isSteepSlope && isOnPlatform)
		{
			Vector directedPlatformVector = platformVector.getVy() > 0.0f ? -platformVector : platformVector;

			// BRODER
			_velocity = directedPlatformVector * 500.0f;
		}
	}

	void DynamicBody::changePosition(const Vector& offset)
	{
		const Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		Point newPosition = position + offset;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
	}

	bool DynamicBody::detectCollision(void* caller, void* data, const StaticBody& staticBody)
	{
		Vector& collision = *static_cast<Vector*>(data);
		DynamicBody* dynamicBody = static_cast<DynamicBody*>(caller);
		return dynamicBody->detectCollision(staticBody, collision);
	}
}