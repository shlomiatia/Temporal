#include "DynamicBody.h"
#include "Grid.h"
#include "BaseUtils.h"
#include "Serialization.h"
#include "Math.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "CollisionInfo.h"
#include "PhysicsEnums.h"
#include "MessageUtils.h"
#include <algorithm>

namespace Temporal
{
	static const int COLLISION_MASK = FilterType::OBSTACLE;

	static const Hash IS_GRAVITY_ENABLED_SERIALIZATION = Hash("DYN_SER_IS_GRAVITY_ENABLED");
	static const NumericPairSerializer VELOCITY_SERIALIZER("DYN_SER_VELOCITY");

	const Vector DynamicBody::GRAVITY(0.0f, -4350.0f);

	float getMaxMovementStepSize(const CollisionInfo& info)
	{
		const Shape& shape = info.getLocalShape();
		float maxHorizontalStepSize = shape.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = shape.getHeight() / 2.0f - 1.0f;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	DynamicBody::DynamicBody(CollisionInfo* info)
		: _collisionInfo(info), _velocity(Vector::Zero), _absoluteImpulse(Vector::Zero), _gravityEnabled(true), _groundVector(Vector::Zero), MAX_MOVEMENT_STEP_SIZE(getMaxMovementStepSize(*info)) 
	{
	}

	void DynamicBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SHAPE)
		{
			Shape* shape = const_cast<Shape*>(&_collisionInfo->getGlobalShape());
			message.setParam(shape);
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			Graphics::get().draw(_collisionInfo->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND_VECTOR)
		{
			message.setParam(&_groundVector);
		}
		else if(message.getID() == MessageID::SET_TIME_BASED_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			Vector impulse = Vector(param.getVx() * getOrientation(*this), param.getVy());

			// If moving horizontally on the ground, we adjust to movement according to the ground vector, because we do want no slow downs on moderate slopes
			if(impulse.getVy() == 0.0f && impulse.getVx() != 0.0f && _groundVector != Vector::Zero)
			{
				impulse = (impulse.getVx() > 0.0f ? _groundVector : -_groundVector) * impulse.getLength();
			}

			// We never want to accumalate horizontal speed from the outside. However, vertical speed need to be accumalted on steep slopes
			_velocity.setVx(0.0f);
			_velocity += impulse;
		}
		else if(message.getID() == MessageID::SET_ABSOLUTE_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			_absoluteImpulse = Vector(param.getVx() * getOrientation(*this), param.getVy());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = getBoolParam(message.getParam());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = getFloatParam(message.getParam());
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			serialization.serialize(IS_GRAVITY_ENABLED_SERIALIZATION, _gravityEnabled);
			VELOCITY_SERIALIZER.serialize(serialization, _velocity);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			_gravityEnabled = serialization.deserializeBool(IS_GRAVITY_ENABLED_SERIALIZATION);
			VELOCITY_SERIALIZER.deserialize(serialization, _velocity);
		}
	}

	void DynamicBody::update(float framePeriodInMillis)
	{
		_collisionInfo->update();
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
			// Apply gravity if needed
			if(_gravityEnabled)
			{
				_velocity += GRAVITY * interpolation;
			}
			
			movement = _velocity * interpolation;
		}
		
		
		return movement;
	}

	void DynamicBody::executeMovement(Vector movement)
	{
		Vector collision = Vector::Zero;
		_groundVector = Vector::Zero;

		AABB dynamicBodyBounds = static_cast<const AABB&>(_collisionInfo->getGlobalShape());

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
			dynamicBodyBounds.translate(stepMovement);
			
			CollisionInfoCollection info = Grid::get().iterateTiles(dynamicBodyBounds, COLLISION_MASK);
			for(CollisionInfoIterator i = info.begin(); i != info.end(); ++i)
			{
				const Shape& staticBodyBounds = (**i).getGlobalShape();
				detectCollision(dynamicBodyBounds, staticBodyBounds, collision);
			}
			if(collision != Vector::Zero)
				break;
		}
		sendMessageToOwner(Message(MessageID::SET_POSITION, const_cast<Point*>(&dynamicBodyBounds.getCenter())));
		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &collision));
		_collisionInfo->update();
		// Absolute impulses last one frame
		_absoluteImpulse = Vector::Zero;
	}

	void DynamicBody::detectCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& collision)
	{
		Vector correction = Vector::Zero;
		if(intersects(dynamicBodyBounds, staticBodyBounds, &correction))
		{
			correctCollision(dynamicBodyBounds, staticBodyBounds, correction, collision);
		}
	}

	void DynamicBody::correctCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision)
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

		dynamicBodyBounds.translate(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(const Shape& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope)
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

	void DynamicBody::modifyVelocity(const Shape& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope)
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
}