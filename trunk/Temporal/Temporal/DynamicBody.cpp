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
			Grid::get().add(_fixture);
			_maxMovementStepSize = getMaxMovementStepSize(*_fixture);
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			YABP* shape = &_fixture->getGlobalShape();
			message.setParam(shape);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND)
		{
			if(_ground)
				message.setParam(const_cast<YABP*>(_ground));
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
		else if(_ground && !isModerateAngle(_ground->getSlopedRadius().getAngle()))
		{
			_velocity = _ground->getSlopedRadius().normalize() * 500.0f;
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
		if(_velocity.getX() == 0)
				return;
		
		// Calculate platform touch point, or front if flat
		const YABP& dynamicBodyBounds = _fixture->getGlobalShape();
		Side::Enum side = Side::get(_velocity.getX());
		Vector direction = _ground->getSlopedRadius().normalize() * static_cast<float>(side);
		Vector curr = Vector(direction.getY() > 0.0f ? dynamicBodyBounds.getSide(side) : dynamicBodyBounds.getSide(Side::getOpposite(side)), dynamicBodyBounds.getBottom());

		float movementAmount = _velocity.getLength();
		Vector velocity = _velocity;
		// /\ When trasnitioning to downward slope we can stuck, so don't modify velocity in this case
		if(direction.getY() >= 0.0 || (curr.getX() - _ground->getSide(Side::getOpposite(side))) * side >= 0.0f)
			velocity = direction * movementAmount;

		// When falling from downward slope, it's look better to fall in the direction of the platform. This is not the case for upward slopes
		if(direction.getY() <= 0.0f )
			_velocity = velocity;

		Vector movement = velocity * framePeriod;						
		Vector dest = curr + movement;
		Vector max = _ground->getTop(side);

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
			FixtureCollection info = Grid::get().iterateTiles(checker, COLLISION_MASK);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const YABP* next = &(**i).getGlobalShape();
				Vector newDirection = next->getSlopedRadius().normalize() * static_cast<float>(side);
				if((next->getSide(side) - max.getX()) * side > 0.0f && isModerateAngle(newDirection.getAngle()) && intersects(checker, *next))
					_ground = next;
			}

			if(!_ground)
			{
				executeMovement(movement);
			}
			else
			{
				Vector oldMovement = max - curr;
				float movementLeft = movementAmount * framePeriod - oldMovement.getLength();
				Vector newDirection = _ground->getSlopedRadius().normalize() * static_cast<float>(side);

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
		bool detectingGround = false;

		YABP dynamicBodyBounds = _fixture->getGlobalShape();
		YABP previous = dynamicBodyBounds;

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
				const YABP& staticBodyBounds = (**i).getGlobalShape();
				detectCollision(dynamicBodyBounds, staticBodyBounds, collision, detectingGround);
			}
			if(collision != Vector::Zero)
				break;
		}
		raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&dynamicBodyBounds.getCenter())));
		
		if(_ground)
		{
			_velocity = Vector::Zero;
		}
		raiseMessage(Message(MessageID::BODY_COLLISION, &collision));
		_fixture->update();
		Grid::get().update(previous, _fixture);

		// Absolute impulses last one frame
		_absoluteImpulse = Vector::Zero;
	}

	void DynamicBody::detectCollision(YABP& dynamicBodyBounds, const YABP& staticBodyBounds, Vector& collision, bool& detectingGround)
	{
		Vector correction = Vector::Zero;
		if(intersects(dynamicBodyBounds, staticBodyBounds, &correction))
		{
			correctCollision(dynamicBodyBounds, staticBodyBounds, correction, collision, detectingGround);
		}
	}

	void DynamicBody::correctCollision(YABP& dynamicBodyBounds, const YABP& staticBodyBounds, Vector& correction, Vector& collision, bool& detectingGround)
	{
		modifyCorrection(dynamicBodyBounds, staticBodyBounds, correction);
		modifyVelocity(correction);

		// If got collision from below, calculate ground vector
		if(correction.getY() > 0.0f/* || (detectingGround && correction.getY() >= 0.0f && _ground->getBottom() < staticBodyBounds.getBottom())*/)
		{
			detectingGround = true;
			_ground = &staticBodyBounds;
		}

		dynamicBodyBounds.translate(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(const YABP& dynamicBodyBounds, const YABP& staticBodyBounds, Vector& correction)
	{
		bool isOnPlatformLeft = dynamicBodyBounds.getLeft() <= staticBodyBounds.getLeft();
		bool isOnPlatformRight = dynamicBodyBounds.getRight() >= staticBodyBounds.getRight();
		bool isOnPlatformSide = isOnPlatformLeft || isOnPlatformRight;

		// If actor don't want to move horizontally, we allow to correct by y if small enough. This is good to prevent falling from edges
		if(isOnPlatformSide && abs(_velocity.getY()) > EPSILON && correction.getX() != 0.0f) 
		{	
			float y = 0.0f;
			y = isOnPlatformLeft ? staticBodyBounds.getTopLeft().getY() : staticBodyBounds.getTopRight().getY();
			float yCorrection = y - dynamicBodyBounds.getBottom();

			// BRODER
			if(yCorrection > 0.0f && yCorrection < 10.0f)
				correction = Vector(0, yCorrection);
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