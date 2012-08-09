#include "Laser.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Grid.h"
#include "Math.h"
#include "ShapeOperations.h"
#include "NumericPair.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	static const Hash DIRECTION_SERIALIZATION = Hash("LAS_SER_DIR");

	void Laser::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::LEVEL_CREATED)
		{
			const Segment& segment = *static_cast<Segment*>(EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_SHAPE)));
			Point position = segment.getNaturalOrigin();
			position.setY(position.getY() - 1.0f);
			sendMessageToOwner(Message(MessageID::SET_POSITION, &position));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *static_cast<float*>(message.getParam());			
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			serialization.serialize(DIRECTION_SERIALIZATION, _isPositiveDirection);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			_isPositiveDirection = serialization.deserializeBool(DIRECTION_SERIALIZATION);
		}
	}

	void Laser::update(float framePeriodInMillis)
	{
		const Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
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
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
		Point pointOfIntersection = Point::Zero;
		Grid::get().cast(newPosition, laserVector, pointOfIntersection);
		Segment seg = SegmentPP(newPosition, pointOfIntersection);
		bool isDetecting = false;
		const AABB& rect = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_SHAPE)));
		isDetecting = intersects(rect, seg);
		Color color = isDetecting ? Color::Green : Color::Red;
		sendMessageToOwner(Message(MessageID::SET_COLOR, &color));
		sendMessageToOwner(Message(MessageID::SET_TARGET, &pointOfIntersection));
	}
}