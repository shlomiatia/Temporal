#include "Laser.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Grid.h"
#include "Math.h"
#include "ShapeOperations.h"
#include "NumericPair.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "Fixture.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	static const int COLLISION_MASK = FilterType::OBSTACLE | FilterType::PLAYER;

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	static const Hash DIRECTION_SERIALIZATION = Hash("LAS_SER_DIR");

	void Laser::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::LEVEL_CREATED)
		{
			const Segment& segment = *static_cast<Segment*>(EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_SHAPE)));
			Point position = segment.getNaturalOrigin();
			position.setY(position.getY() - 1.0f);
			raiseMessage(Message(MessageID::SET_POSITION, &position));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = getFloatParam(message.getParam());
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			serialization.serialize(DIRECTION_SERIALIZATION, _isPositiveDirection);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			_isPositiveDirection = serialization.deserializeBool(DIRECTION_SERIALIZATION);
		}
	}

	void Laser::update(float framePeriodInMillis)
	{
		const Point& position = getPosition(*this);
		const Segment& segment = *static_cast<Segment*>(EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_SHAPE)));
		Vector directionVector = segment.getNaturalVector().normalize();
		Vector laserVector = directionVector.getRightNormal();
		float movementAmount = LASER_SPEED_PER_SECOND * framePeriodInMillis / 1000.0f;
		Point maxPoint = Point::Zero;
		if(_isPositiveDirection)
		{
			maxPoint = segment.getNaturalTarget();
		}
		else
		{
			maxPoint = segment.getNaturalOrigin();
			directionVector = -directionVector;
		}
		Vector movement = directionVector * movementAmount;
		Vector currDiff = maxPoint - position;
		Vector nextDiff = currDiff + movement;
		if(!sameSign(currDiff.getVx(), nextDiff.getVx()) || !sameSign(currDiff.getVy(), nextDiff.getVy()))
		{
			_isPositiveDirection = !_isPositiveDirection;
		}
		Point newPosition = position + movement;
		raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
		RayCastResult result;
		int group = *static_cast<int*>(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if(Grid::get().cast(newPosition, laserVector, result, COLLISION_MASK, group))
		{
			Color color = result.getFixture().getEntityId() == PLAYER_ENTITY ? Color::Green : Color::Red;
			raiseMessage(Message(MessageID::SET_COLOR, &color));
			raiseMessage(Message(MessageID::SET_TARGET, const_cast<Point*>(&result.getPoint())));
		}
	}
}