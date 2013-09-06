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
	const Hash DynamicBody::TYPE = Hash("dynamic-body");

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE;

	Vector DynamicBody::GRAVITY(0.0f, -1500.0f);

	float getMaxMovementStepSize(const Fixture& fixture)
	{
		const YABP& shape = fixture.getLocalShape();
		float maxHorizontalStepSize = shape.getWidth() / 2.0f - 1.0f;
		float maxVerticalStepSize = shape.getHeight() / 2.0f - 1.0f;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	DynamicBody::~DynamicBody()
	{
		delete _fixture;
	}

	Component* DynamicBody::clone() const { return new DynamicBody(_fixture->clone()); }

	void DynamicBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_fixture->init(*this);
			getEntity().getManager().getGameState().getGrid().add(_fixture);
			_maxMovementStepSize = getMaxMovementStepSize(*_fixture);
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			YABP* shape = &_fixture->getGlobalShape();
			message.setParam(shape);
		}
		else if(message.getID() == MessageID::GET_VELOCITY)
		{
			message.setParam(&_velocity);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND)
		{
			if(_ground)
				message.setParam(const_cast<Fixture*>(_ground));
		}
		else if(message.getID() == MessageID::SET_TIME_BASED_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			Vector impulse = Vector(param.getX() * getOrientation(*this), param.getY());
			_velocity = impulse;
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
		else if(message.getID() == MessageID::SET_POSITION)
		{
			getEntity().getManager().getGameState().getGrid().update(_fixture);
		}
	}

	void DynamicBody::update(float framePeriod)
	{
		_fixture->update();

		// Absolute impulse
		if(_absoluteImpulse != Vector::Zero)
		{
			_ground = 0;
			executeMovement(_absoluteImpulse);
		}
		// Slide
		else if(_ground && !isModerateAngle(_ground->getGlobalShape().getSlopedRadius().getAngle()))
		{
			// BRODER
			_velocity = _ground->getGlobalShape().getSlopedRadius().normalize() * 250.0f;
			if(_velocity .getY() > 0.0f)
				_velocity = -_velocity;
			_ground = 0;
			executeMovement(determineMovement(framePeriod));
		}
		// Walk
		else if(_ground && _velocity.getY() == 0.0f)
		{
			walk(framePeriod);
		}
		// Air
		else
		{
			_ground = 0;
			executeMovement(determineMovement(framePeriod));
		}
	}

	void DynamicBody::walk(float framePeriod)
	{
		const YABP& dynamicBodyBounds = _fixture->getGlobalShape();
		const YABP& staticBodyBounds = _ground->getGlobalShape();

		// Moving platform
		if(staticBodyBounds.getCenter() != _previousGroundCenter)
		{
			Vector newPosition = dynamicBodyBounds.getCenter() + staticBodyBounds.getCenter() - _previousGroundCenter;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
		}
		
		// Fix when static
		if(_velocity.getX() == 0)
		{
			_ground = 0;
			executeMovement(determineMovement(framePeriod));
			return;
		}
		
		// Calculate platform touch point, or front if flat
		Side::Enum side = Side::get(_velocity.getX());
		Vector direction = _ground->getGlobalShape().getSlopedRadius().normalize() * static_cast<float>(side);
		Vector curr = Vector(direction.getY() > 0.0f ? dynamicBodyBounds.getSide(side) : dynamicBodyBounds.getSide(Side::getOpposite(side)), dynamicBodyBounds.getBottom());

		float movementAmount = _velocity.getLength();
		Vector velocity = _velocity;

		// /\ When trasnitioning to downward slope we can stuck, so don't modify velocity in this case
		if(direction.getY() >= 0.0 || (curr.getX() - _ground->getGlobalShape().getSide(Side::getOpposite(side))) * side >= 0.0f)
			velocity = direction * movementAmount;

		Vector movement = velocity * framePeriod;						
		Vector dest = curr + movement;
		Vector max = _ground->getGlobalShape().getTop(side);

		// Still on platform
		if((dest.getX() - max.getX()) * side <= 0.0f)
		{
			executeMovement(movement);
		}
		else
		{
			// Find next platform
			_ground = 0;
			YABP checker = YABPAABB(max, Vector(1.0f, 1.0f));
			FixtureCollection info = getEntity().getManager().getGameState().getGrid().iterateTiles(checker, COLLISION_MASK);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const Fixture* next = *i;
				Vector newDirection = next->getGlobalShape().getSlopedRadius().normalize() * static_cast<float>(side);
				if((next->getGlobalShape().getSide(side) - max.getX()) * side > 0.0f && isModerateAngle(newDirection.getAngle()))
				{
					_ground = next;
				}
					
			}

			// Fall
			if(!_ground)
			{
				// When falling from downward slope, it's look better to fall in the direction of the platform. This is not the case for upward slopes
				if(direction.getY() <= 0.0f )
					_velocity = velocity;
				executeMovement(movement);
			}
			else
			{
				Vector oldMovement = max - curr;
				float movementLeft = movementAmount * framePeriod - oldMovement.getLength();
				Vector newDirection = _ground->getGlobalShape().getSlopedRadius().normalize() * static_cast<float>(side);

				// /\ \/ Fix of this transitions
				if(differentSign(direction.getY(), newDirection.getY()))
				{
					newDirection = Vector(static_cast<float>(side), 0.0f);
				}
				Vector newMovement = movementLeft * newDirection;
				movement = newMovement + oldMovement;
				executeMovement(movement);
			}
		}
	}

	Vector DynamicBody::determineMovement(float framePeriod)
	{
		if(_gravityEnabled)
		{
			_velocity += GRAVITY * framePeriod;
		}
			
		Vector movement = _velocity * framePeriod;
		return movement;
	}

	void DynamicBody::executeMovement(Vector movement)
	{
		Vector collision = Vector::Zero;
		YABP dynamicBodyBounds = _fixture->getGlobalShape();
		YABP previous = dynamicBodyBounds;

		// If the movement is too big, we'll divide it to smaller steps
		do
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
			
			FixtureCollection info = getEntity().getManager().getGameState().getGrid().iterateTiles(dynamicBodyBounds, COLLISION_MASK, -1, false);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const Fixture* fixture = *i;
				detectCollision(dynamicBodyBounds, fixture, collision, stepMovement);
			}
			if(collision != Vector::Zero)
				break;
		}
		while(movement != Vector::Zero);

		raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&dynamicBodyBounds.getCenter())));
		
		if(_ground)
		{
			_velocity = Vector::Zero;
			_previousGroundCenter = _ground->getGlobalShape().getCenter();
		}
		raiseMessage(Message(MessageID::BODY_COLLISION, &collision));

		// Absolute impulses last one frame
		_absoluteImpulse = Vector::Zero;
	}

	void DynamicBody::detectCollision(YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& collision, Vector& movement)
	{
		Vector correction = Vector::Zero;
		if(intersects(dynamicBodyBounds, staticBodyBounds->getGlobalShape(), &correction))
		{
			correctCollision(dynamicBodyBounds, staticBodyBounds, correction, collision, movement);
		}
	}

	void DynamicBody::correctCollision(YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement)
	{
		modifyCorrection(dynamicBodyBounds, staticBodyBounds, correction, movement);
		modifyVelocity(correction);
		
		Side::Enum side = getOrientation(*this);

		// If got collision from below, calculate ground vector. Take front ground when there is a dellima
		if(correction.getY() > 0.0f ||
			(correction.getY() >= 0.0f &&
		    _ground &&
			(staticBodyBounds->getGlobalShape().getSide(side) - _ground->getGlobalShape().getSide(side)) * side > 0.0f) &&
			isModerateAngle(staticBodyBounds->getGlobalShape().getSlopedRadius().getAngle()))
		{
			_ground = staticBodyBounds;
		}

		dynamicBodyBounds.translate(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(const YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& movement)
	{
		// If actor don't want to move horizontally, we allow to correct by y if small enough. This is good to prevent falling from edges
		if(abs(_velocity.getY()) > EPSILON && abs(correction.getX()) > EPSILON) 
		{	
			const YABP& shape = staticBodyBounds->getGlobalShape();
			Vector normalizedRadius = shape.getSlopedRadius().normalize();
			float x = normalizedRadius.getY() >= 0.0f ? dynamicBodyBounds.getRight() : dynamicBodyBounds.getLeft();
			float length = (x - shape.getCenter().getX()) / normalizedRadius.getX();
			float y = shape.getCenter().getY() + shape.getYRadius() + normalizedRadius.getY() * length;
			
			float yCorrection = y - dynamicBodyBounds.getBottom();

			// BRODER
			if(yCorrection > 0.0f && yCorrection < 10.0f)
				correction = Vector(0, yCorrection);
		}
		// Don't allow to fix into ground _* \ 
		else if(correction.getY() < -EPSILON && _ground)
		{
			correction = -movement;
		}
	}

	void DynamicBody::modifyVelocity(const Vector& correction)
	{
		// Stop the actor where the correction was applied
		if(differentSign(correction.getX(), _velocity.getX()))
			_velocity.setX(0.0f);
		if(differentSign(correction.getY(), _velocity.getY()))
			_velocity.setY(0.0f);
	}
}