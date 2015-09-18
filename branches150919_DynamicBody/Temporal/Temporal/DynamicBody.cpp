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
#include "Utils.h"
#include <algorithm>

namespace Temporal
{
	const Hash DynamicBody::TYPE = Hash("dynamic-body");

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE;

	Vector DynamicBody::GRAVITY(0.0f, -1200.0f);

	float getMaxMovementStepSize(const Fixture& fixture)
	{
		const OBB& shape = fixture.getLocalShape();
		float maxHorizontalStepSize = shape.getRadiusX() - EPSILON;
		float maxVerticalStepSize = shape.getRadiusY() - EPSILON;
		if (maxHorizontalStepSize <= 0.0f)
			return maxVerticalStepSize;
		if (maxVerticalStepSize <= 0.0f)
			return maxHorizontalStepSize;
		return std::min(maxHorizontalStepSize, maxVerticalStepSize);
	}

	Segment getTopSegment(const OBB& shape, float leftX, float rightX)
	{
		if (shape.getAxisX().getX() == 0.0f || shape.getAxisX().getY() == 0.0f)
		{
			return SegmentPP(shape.getCenter() + Vector(-shape.getRadiusX(), shape.getRadiusY()), shape.getCenter() + shape.getRadius());
		}
		Vector topPoint = shape.getCenter() +
			(shape.getAxisX().getY() > 0.0f ? shape.getAxisX() : -shape.getAxisX()) * shape.getRadiusX() +
			(shape.getAxisY().getY() > 0.0f ? shape.getAxisY() : -shape.getAxisY()) * shape.getRadiusY();
		if (rightX < topPoint.getX())
		{
			Vector leftPoint = shape.getCenter() -
				(shape.getAxisX().getX() > 0.0f ? shape.getAxisX() : -shape.getAxisX()) * shape.getRadiusX() -
				(shape.getAxisY().getX() > 0.0f ? shape.getAxisY() : -shape.getAxisY()) * shape.getRadiusY();
			return SegmentPP(leftPoint, topPoint);
		}
		else if (leftX > topPoint.getX())
		{
			Vector rightPoint = shape.getCenter() +
				(shape.getAxisX().getX() > 0.0f ? shape.getAxisX() : -shape.getAxisX()) * shape.getRadiusX() +
				(shape.getAxisY().getX() > 0.0f ? shape.getAxisY() : -shape.getAxisY()) * shape.getRadiusY();
			return SegmentPP(topPoint, rightPoint);
		}
		else
		{
			return SegmentPP(topPoint, topPoint);
		}
	}

	Segment getTopSegment(const OBB& shape, float x)
	{
		return getTopSegment(shape, x, x);
	}

	DynamicBody::~DynamicBody()
	{
		delete _fixture;
	}

	Component* DynamicBody::clone() const { return new DynamicBody(_fixture->clone(), _gravityEnabled, _collisionMask); }

	void DynamicBody::handleMessage(Message& message)
	{
		handleGridFixtureMessage(message, *this, *_fixture);
		if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_maxMovementStepSize = getMaxMovementStepSize(*_fixture);
			_collisionMask |= COLLISION_MASK;
		}
		else if(message.getID() == MessageID::GET_VELOCITY)
		{
			message.setParam(&_velocity);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().getLinesSpriteBatch().add(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND)
		{
			message.setParam(const_cast<Fixture*>(_ground));
		}
		else if(message.getID() == MessageID::SET_GROUND)
		{
			_ground = static_cast<Fixture*>(message.getParam());
			_groundId = _ground->getEntityId();
			_previousGroundCenter = _ground->getGlobalShape().getCenter();
		}
		else if(message.getID() == MessageID::SET_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			Vector impulse = Vector(param.getX() * getOrientation(*this), param.getY());
			_velocity = impulse;
			
		}
		else if(message.getID() == MessageID::SET_BODY_ENABLED)
		{
			_fixture->setEnabled(getBoolParam(message.getParam()));
			_velocity = Vector::Zero;
		}
		else if (message.getID() == MessageID::SET_GRAVITY_ENABLED)
		{
			_gravityEnabled= getBoolParam(message.getParam());
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
		else if (message.getID() == MessageID::POST_LOAD)
		{
			if (_groundId == Hash::INVALID)
			{
				resetGround();
			}
			else 
			{
				_ground = static_cast<Fixture*>(getEntity().getManager().sendMessageToEntity(_groundId, Message(MessageID::GET_FIXTURE)));
			}
		}
		else if (message.getID() == MessageID::TEMPORAL_PERIOD_CHANGED)
		{
			if (_ground && !_ground->canCollide(CollisionCategory::OBSTACLE, getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)))))
			{
				resetGround();
			}
		}
		else if (message.getID() == MessageID::TEMPORAL_ECHOS_MERGED)
		{
			if (_ground && _ground->getCategory() & CollisionCategory::DRAGGABLE)
			{
				resetGround();
			}
		}
	}

	void DynamicBody::update(float framePeriod)
	{
		OBB globalShapeClone = _fixture->getGlobalShape();
		OBBAABBWrapper dynamicBodyBounds(&globalShapeClone);

		// Moving platform - position is set later
		if(_ground && _ground->getGlobalShape().getCenter() != _previousGroundCenter)
		{
			Vector vector = _ground->getGlobalShape().getCenter() - _previousGroundCenter;
			if (_velocity.getX() == 0.0f || sameSign(_velocity.getX(), vector.getX()))
				dynamicBodyBounds.getOBB().translate(vector);
		}
			
		if(_fixture->isEnabled())
		{
			Segment groundSegment;
			if (_ground)
				groundSegment = getTopSegment(_ground->getGlobalShape(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight());

			// Slide
			if(_ground && !AngleUtils::radian().isModerate(groundSegment.getRadius().getAngle()))
			{
				// BRODER
				_velocity = groundSegment.getNaturalDirection() * 375.0f;
				if(_velocity.getY() > 0.0f)
					_velocity = -_velocity;
				_ground = 0;
				_groundId = Hash::INVALID;
				executeMovement(dynamicBodyBounds, determineMovement(framePeriod));
			}
			// Walk
			else if(_ground && _velocity.getY() == 0.0f)
			{
				walk(dynamicBodyBounds, framePeriod);
			}
			// Air
			else
			{
				_ground = 0;
				_groundId = Hash::INVALID;
				executeMovement(dynamicBodyBounds, determineMovement(framePeriod));
			}
		}

		raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&dynamicBodyBounds.getCenter())));
		
		if(_ground)
		{
			_velocity = Vector::Zero;
			_previousGroundCenter = _ground->getGlobalShape().getCenter();
		}
	}

	bool DynamicBody::transitionPlatform(OBBAABBWrapper& dynamicBodyBounds, const Vector& direction, Side::Enum side, float leftPeriod)
	{
		const float MAX_DISTANCE = 10.0f;

		Side::Enum oppositeSide = Side::getOpposite(side);
		Vector bodyPoint = Vector(direction.getY() > 0.0f ? dynamicBodyBounds.getSide(side) : dynamicBodyBounds.getSide(oppositeSide), dynamicBodyBounds.getBottom());
		Vector rayOrigin = bodyPoint + Vector(dynamicBodyBounds.getRadiusX() * side, 0.0f);
		RayCastResult result;
		int sourceCollisionGroup = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if (getEntity().getManager().getGameState().getGrid().cast(rayOrigin, Vector(0.0f, -1.0f), result, _collisionMask, sourceCollisionGroup) &&
			(result.getPoint() - rayOrigin).getLength() < MAX_DISTANCE)
		{
			Segment groundSegment = getTopSegment(result.getFixture().getGlobalShape(), rayOrigin.getX());
			Vector groundDirection = groundSegment.getNaturalDirection();
			Vector distanceFromPlatform = groundSegment.getPoint(oppositeSide) + Vector(groundDirection.getX() * side, groundDirection.getY()) - bodyPoint;
			if (distanceFromPlatform.getLength() < MAX_DISTANCE && AngleUtils::radian().isModerate(groundDirection.getAngle()))
			{
				_ground = &result.getFixture();
				_groundId = result.getFixture().getEntityId();
				_previousGroundCenter = _ground->getGlobalShape().getCenter();
				dynamicBodyBounds.getOBB().translate(distanceFromPlatform);
				raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&dynamicBodyBounds.getCenter())));
				walk(dynamicBodyBounds, leftPeriod);
				return true;
			}
		}
		return false;
	}

	void DynamicBody::walk(OBBAABBWrapper& dynamicBodyBounds, float framePeriod)
	{
		const Segment& groundSegment = getTopSegment(_ground->getGlobalShape(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight());
		const OBB& staticBodyBounds = _ground->getGlobalShape();
		
		// Fix when static
		if(_velocity.getX() == 0)
		{
			_ground = 0;
			_groundId = Hash::INVALID;
			executeMovement(dynamicBodyBounds, determineMovement(framePeriod));
			return;
		}
		
		// Calculate platform touch point, or front if flat
		Side::Enum side = Side::get(_velocity.getX());
		Side::Enum oppositeSide = Side::getOpposite(side);

		// /\ When trasnitioning to downward slope we can stuck, so don't modify velocity in this case
		Vector direction = (groundSegment.getRadius() == Vector::Zero ? Vector(1.0f, 0.0f) : groundSegment.getNaturalDirection()) * static_cast<float>(side);
		Vector curr = Vector(direction.getY() > 0.0f ? dynamicBodyBounds.getSide(side) : dynamicBodyBounds.getSide(oppositeSide), dynamicBodyBounds.getBottom());

		float movementAmount = _velocity.getLength();
		Vector velocity = _velocity;

		velocity = direction * movementAmount;

		Vector movement = velocity * framePeriod;						
		Vector dest = curr + movement;
		Vector max = groundSegment.getPoint(side);
		
		// Still on platform
		if((dest.getX() - max.getX()) * side <= 0.0f)
		{
			executeMovement(dynamicBodyBounds, movement);
		}
		else
		{			
			Vector actualMovement = max - curr;
			executeMovement(dynamicBodyBounds, actualMovement);
			float leftPeriod = framePeriod * (1.0f - actualMovement.getLength() / movement.getLength());
			_velocity = Vector(velocity.getLength() * side, 0.0f);

			if (!transitionPlatform(dynamicBodyBounds, direction, side, leftPeriod))
			{
				resetGround();

				// When falling from downward slope, it's look better to fall in the direction of the platform. This is not the case for upward slopes
				if (direction.getY() <= 0.0f)
					_velocity = velocity;

				Vector movementLeft = determineMovement(leftPeriod);
				executeMovement(dynamicBodyBounds, movementLeft);
			}
		}
	}

	Vector DynamicBody::determineMovement(float framePeriod)
	{
		if (_gravityEnabled)
			_velocity += GRAVITY * framePeriod;
			
		Vector movement = _velocity * framePeriod;
		return movement;
	}

	void DynamicBody::executeMovement(OBBAABBWrapper& dynamicBodyBounds, Vector movement)
	{
		Vector collision = Vector::Zero;

		// If the movement is too big, we'll divide it to smaller steps
		do
		{
			Vector stepMovement = Vector::Zero;
			float movementAmount = movement.getLength();

			if(movementAmount <= _maxMovementStepSize)
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
			dynamicBodyBounds.getOBB().translate(stepMovement);
			
			
			FixtureList info = getEntity().getManager().getGameState().getGrid().iterateTiles(dynamicBodyBounds.getOBB(), _collisionMask, _fixture->getGroup(), false);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const Fixture* fixture = *i;
				detectCollision(dynamicBodyBounds, fixture, collision, stepMovement);
			}
		}
		while(movement != Vector::Zero);

		modifyVelocity(collision);
		if(collision != Vector::Zero)
			raiseMessage(Message(MessageID::BODY_COLLISION, &collision));
	}

	void DynamicBody::detectCollision(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& collision, Vector& movement)
	{
		Vector correction = Vector::Zero;
		if (_fixture != staticBodyBounds && intersects(dynamicBodyBounds.getOBB(), staticBodyBounds->getGlobalShape(), &correction))
		{
			if(fabsf(correction.getX()) > EPSILON || fabsf(correction.getY()) > EPSILON)
				correctCollision(dynamicBodyBounds, staticBodyBounds, correction, collision, movement);
		}
	}

	void DynamicBody::correctCollision(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement)
	{
		modifyCorrection(dynamicBodyBounds, staticBodyBounds, correction, movement);
		
		dynamicBodyBounds.getOBB().translate(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& movement)
	{
		bool modifyGround = true;

		// Fix minor gaps in transitions /*\ /*- BRODER
		if(_ground && differentSign(movement.getX(), correction.getX()) &&
			staticBodyBounds->getGlobalShape().getTop() - dynamicBodyBounds.getBottom() < 5.0f && staticBodyBounds->getGlobalShape().getTop() - dynamicBodyBounds.getBottom() > EPSILON)
		{
			correction = Vector(0, staticBodyBounds->getGlobalShape().getTop() - dynamicBodyBounds.getBottom());
		}
		if(_ground && 
				// Don't allow to fix into ground _*\ 
				(correction.getY() < -EPSILON || 
				// Don't climb on steep slope _*/
				(differentSign(movement.getX(), correction.getX()) && AngleUtils::radian().isSteep(correction.getRightNormal().getAngle()))))
		{
 			correction = -movement;
			modifyGround = false;
		}
		// If entity is falling, we allow to correct by y if small enough. This is good to prevent falling from edges, and sliding on moderate slopes
		if(abs(_velocity.getY()) > EPSILON && abs(correction.getX()) > EPSILON && AngleUtils::radian().isModerate(correction.getRightNormal().getAngle()))
		{	
			Segment shape = getTopSegment(staticBodyBounds->getGlobalShape(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight());
			Vector normalizedRadius = shape.getRadius() == Vector::Zero ? Vector(1.0f, 0.0f) : shape.getNaturalDirection();
			float x = normalizedRadius.getY() >= 0.0f ? dynamicBodyBounds.getRight() : dynamicBodyBounds.getLeft();
			float length = (x - shape.getCenter().getX()) / normalizedRadius.getX();
			float y = shape.getCenter().getY() + normalizedRadius.getY() * length;
			
			float yCorrection = y - dynamicBodyBounds.getBottom();

			// BRODER
			if(yCorrection > 0.0f && yCorrection < 10.0f) {
				correction = Vector(0, yCorrection);
			}
		}

		// If got collision from below, calculate ground vector. Take front ground when there is a dellima
		if(correction.getY() > 0.0f && modifyGround)
		{
			_ground = staticBodyBounds;
			_groundId = staticBodyBounds->getEntityId();
		}
		
	}

	void DynamicBody::modifyVelocity(const Vector& collision)
	{
		// Stop the actor where the correction was applied
		if (sameSign(collision.getX(), _velocity.getX()))
			_velocity.setX(0.0f);
		if (sameSign(collision.getY(), _velocity.getY()))
			_velocity.setY(0.0f);
	}

	void DynamicBody::resetGround()
	{
		_ground = 0;
		_groundId = Hash::INVALID;
		_previousGroundCenter = Vector::Zero;
		raiseMessage(Message(MessageID::LOST_GROUND));
	}
}