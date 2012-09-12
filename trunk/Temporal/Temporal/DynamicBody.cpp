#include "DynamicBody.h"
#include "Grid.h"
#include "Serialization.h"
#include "Math.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "Fixture.h"
#include "PhysicsEnums.h"
#include "MessageUtils.h"
#include <algorithm>

namespace Temporal
{
	static const int COLLISION_MASK = CollisionCategory::OBSTACLE;

	const Vector DynamicBody::GRAVITY(0.0f, -1500.0f);

	float getMaxMovementStepSize(const Fixture& fixture)
	{
		const Shape& shape = fixture.getLocalShape();
		float maxHorizontalStepSize = shape.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = shape.getHeight() / 2.0f - 1.0f;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	void DynamicBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_fixture->init(*this);
			Grid::get().add(_fixture);
			_maxMovementStepSize = getMaxMovementStepSize(*_fixture);
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			Shape* shape = const_cast<Shape*>(&_fixture->getGlobalShape());
			message.setParam(shape);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND_VECTOR)
		{
			message.setParam(&_groundVector);
		}
		else if(message.getID() == MessageID::SET_TIME_BASED_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			Vector impulse = Vector(param.getX() * getOrientation(*this), param.getY());

			if(impulse.getY() != 0.0f)
				_velocity.setY(0.0f);

			// If moving horizontally on the ground, we adjust to movement according to the ground vector, because we do want no slow downs on moderate slopes
			if(impulse.getY() == 0.0f && impulse.getX() != 0.0f && _groundVector != Vector::Zero)
			{
				impulse = (impulse.getX() > 0.0f ? _groundVector : -_groundVector) * impulse.getLength();
			}

			// We never want to accumalate horizontal speed from the outside. However, vertical speed need to be accumalted on steep slopes
			_velocity.setX(0.0f);
			_velocity += impulse;
		}
		else if(message.getID() == MessageID::SET_ABSOLUTE_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			_absoluteImpulse = Vector(param.getX() * getOrientation(*this), param.getY());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled = getBoolParam(message.getParam());
			_velocity = Vector::Zero;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
	}

	void DynamicBody::update(float framePeriod)
	{
		_fixture->update();
		Vector movement = determineMovement(framePeriod);
		executeMovement(movement);
	}

	Vector DynamicBody::determineMovement(float framePeriod)
	{
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
				_velocity += GRAVITY * framePeriod;
			}
			
			movement = _velocity * framePeriod;
		}
		return movement;
	}

	void DynamicBody::executeMovement(Vector movement)
	{
		Vector collision = Vector::Zero;
		_groundVector = Vector::Zero;

		AABB dynamicBodyBounds = static_cast<const AABB&>(_fixture->getGlobalShape());
		AABB previous = dynamicBodyBounds;

		// If the movement is too big, we'll divide it to smaller steps
		while(movement != Vector::Zero)
		{
			Vector stepMovement = Vector::Zero;
			float movementAmount = movement.getLength();

			// Absolute impulses are done in one stroke
			if(movementAmount <= _maxMovementStepSize || _absoluteImpulse != Vector::Zero)
			{
				stepMovement = movement;
			}
			// Calculate step movement
			else
			{
				float ratio = _maxMovementStepSize / movementAmount;
				stepMovement.setX(movement.getX() * ratio);
				stepMovement.setY(movement.getY() * ratio);
			}
			
			movement -= stepMovement;
			dynamicBodyBounds.translate(stepMovement);
			
			FixtureCollection info = Grid::get().iterateTiles(dynamicBodyBounds, COLLISION_MASK);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const Shape& staticBodyBounds = (**i).getGlobalShape();
				detectCollision(dynamicBodyBounds, staticBodyBounds, collision);
			}
			if(collision != Vector::Zero)
				break;
		}
		raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&dynamicBodyBounds.getCenter())));
		raiseMessage(Message(MessageID::BODY_COLLISION, &collision));
		_fixture->update();
		Grid::get().update(previous, _fixture);
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
		if(correction.getY() >= 0.0f && isModerateSlope)
		{
			if(_groundVector == Vector::Zero || platformVector.getY() == 0.0f)
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
		if(canModifyCorrection && abs(_velocity.getX()) < EPSILON && correction.getX() != 0.0f) 
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
		if(differentSign(correction.getX(), _velocity.getX()) || correction.getY() > 0.0f)
			_velocity.setX(0.0f);
		if(differentSign(correction.getY(), _velocity.getY()))
			_velocity.setY(0.0f);
		else if(_velocity.getY() != 0.0f)
		{
			int i = 0;
		}

		// BRODER
		bool isOnPlatform = dynamicBodyBounds.getRight() > segment.getLeft() &&
							dynamicBodyBounds.getLeft() < segment.getRight() &&
							dynamicBodyBounds.getBottom() + 20.0f <= segment.getTop() &&
							dynamicBodyBounds.getBottom() >= segment.getBottom();
		// Slide on steep slopes
		if(isSteepSlope && isOnPlatform)
		{
			Vector directedPlatformVector = platformVector.getY() > 0.0f ? -platformVector : platformVector;

			// BRODER
			_velocity = directedPlatformVector * 500.0f;
		}
	}
}