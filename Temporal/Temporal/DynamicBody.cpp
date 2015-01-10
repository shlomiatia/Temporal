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

#include "Utils.h"

namespace Temporal
{
	const Hash DynamicBody::TYPE = Hash("dynamic-body");

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE;

	Vector DynamicBody::GRAVITY(0.0f, -1200.0f);

	float getMaxMovementStepSize(const Fixture& fixture)
	{
		const OBB& shape = fixture.getLocalShape();
		float maxHorizontalStepSize = shape.getRadiusX() - 1.0f;
		float maxVerticalStepSize = shape.getRadiusY() - 1.0f;
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
			_dynamicBodyBounds.setOBB(__dynamicBodyBounds);
			getEntity().getManager().getGameState().getGrid().add(_fixture);
			_maxMovementStepSize = getMaxMovementStepSize(*_fixture);
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			message.setParam(&_dynamicBodyBounds);
		}
		else if(message.getID() == MessageID::GET_VELOCITY)
		{
			message.setParam(&_velocity);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().getSpriteBatch().add(_fixture->getGlobalShape(), Color(1.0f, 1.0f, 1.0f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_GROUND)
		{
			if(_ground)
				message.setParam(&_groundSegment);
		}
		else if(message.getID() == MessageID::SET_IMPULSE)
		{
			const Vector& param = getVectorParam(message.getParam());
			Vector impulse = Vector(param.getX() * getOrientation(*this), param.getY());
			_velocity = impulse;
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

	Segment getGroundSegment(const Fixture& ground, OBBAABBWrapper body)
	{
		Vector highPoint = ground.getGlobalShape().getTopRightVertex();
		if(body.getRight() < highPoint.getX())
			return SegmentPP(highPoint, ground.getGlobalShape().getTopLeftVertex());
		else if(body.getLeft() > highPoint.getX())
			return SegmentPP(highPoint, ground.getGlobalShape().getBottomRightVertex());
		else
			return SegmentPP(highPoint, highPoint);
	}

	void DynamicBody::update(float framePeriod)
	{
		_fixture->update();
		__dynamicBodyBounds = _fixture->getGlobalShape();
		if(_ground)
			_groundSegment = getGroundSegment(*_ground, _dynamicBodyBounds);

		// Slide
		if(_ground && !isModerateAngle(_groundSegment.getRadius().getAngle()))
		{
			// BRODER
			_velocity = _groundSegment.getNaturalDirection() * 250.0f;
			if(_velocity.getY() > 0.0f)
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
		const OBB& staticBodyBounds = _ground->getGlobalShape();

		// Moving platform - position is set later (executeWalk)
		if(staticBodyBounds.getCenter() != _previousGroundCenter)
		{
			_dynamicBodyBounds.getOBB().translate(staticBodyBounds.getCenter() - _previousGroundCenter);
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
		Vector direction = (_groundSegment.getRadius() == Vector::Zero ? Vector(1.0f, 0.0f) :  _groundSegment.getNaturalDirection()) * static_cast<float>(side);
		Vector curr = Vector(direction.getY() > 0.0f ? _dynamicBodyBounds.getSide(side) : _dynamicBodyBounds.getSide(Side::getOpposite(side)), _dynamicBodyBounds.getBottom());

		float movementAmount = _velocity.getLength();
		Vector velocity = _velocity;

		// /\ When trasnitioning to downward slope we can stuck, so don't modify velocity in this case
		velocity = direction * movementAmount;

		Vector movement = velocity * framePeriod;						
		Vector dest = curr + movement;
		Vector max = _groundSegment.getPoint(side);

		// Still on platform
		if((dest.getX() - max.getX()) * side <= 0.0f)
		{
			executeMovement(movement);
		}
		else
		{			
			// When falling from downward slope, it's look better to fall in the direction of the platform. This is not the case for upward slopes
			Vector actualMovement = max - curr;
			executeMovement(actualMovement);
			_ground = 0;
			if(direction.getY() <= 0.0f )
				_velocity = velocity;
			float leftPeriod = framePeriod * (1.0 - actualMovement.getLength() / movement.getLength());
			executeMovement(determineMovement(leftPeriod));
			// TODO: Find next platform
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
			_dynamicBodyBounds.getOBB().translate(stepMovement);
			
			FixtureCollection info = getEntity().getManager().getGameState().getGrid().iterateTiles(_dynamicBodyBounds.getOBB(), COLLISION_MASK, -1, false);
			for(FixtureIterator i = info.begin(); i != info.end(); ++i)
			{
				const Fixture* fixture = *i;
				detectCollision(fixture, collision, stepMovement);
			}
			if(collision != Vector::Zero)
				break;
		}
		while(movement != Vector::Zero);

		raiseMessage(Message(MessageID::SET_POSITION, const_cast<Vector*>(&_dynamicBodyBounds.getCenter())));
		
		if(_ground)
		{
			_velocity = Vector::Zero;
			_previousGroundCenter = _ground->getGlobalShape().getCenter();
		}
		if(collision != Vector::Zero)
			raiseMessage(Message(MessageID::BODY_COLLISION, &collision));
	}

	void DynamicBody::detectCollision(const Fixture* staticBodyBounds, Vector& collision, Vector& movement)
	{
		Vector correction = Vector::Zero;
		if(intersects(_dynamicBodyBounds.getOBB(), staticBodyBounds->getGlobalShape(), &correction))
		{
			correctCollision(staticBodyBounds, correction, collision, movement);
		}
	}

	void DynamicBody::correctCollision(const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement)
	{
		modifyCorrection(staticBodyBounds, correction, movement);
		modifyVelocity(correction);
		
		Side::Enum side = getOrientation(*this);

		// If got collision from below, calculate ground vector. Take front ground when there is a dellima
		if(correction.getY() > 0.0f)
		{
			_ground = staticBodyBounds;
		}

		_dynamicBodyBounds.getOBB().translate(correction);
		collision -= correction;
	}

	void DynamicBody::modifyCorrection(const Fixture* staticBodyBounds, Vector& correction, Vector& movement)
	{
		// If actor don't want to move horizontally, we allow to correct by y if small enough. This is good to prevent falling from edges
		if(abs(_velocity.getY()) > EPSILON && abs(correction.getX()) > EPSILON) 
		{	
			Segment shape = getGroundSegment(*staticBodyBounds, _dynamicBodyBounds);
			Vector normalizedRadius = shape.getRadius() == Vector::Zero ? Vector(1.0f, 0.0f) : shape.getNaturalDirection();
			float x = normalizedRadius.getY() >= 0.0f ? _dynamicBodyBounds.getRight() : _dynamicBodyBounds.getLeft();
			float length = (x - shape.getCenter().getX()) / normalizedRadius.getX();
			float y = shape.getCenter().getY() + normalizedRadius.getY() * length;
			
			float yCorrection = y - _dynamicBodyBounds.getBottom();

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