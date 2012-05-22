#include "Laser.h"
#include "Message.h"
#include "Graphics.h"
#include "Segment.h"
#include "EntitiesManager.h"

namespace Temporal
{
	void Laser::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();			
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			Graphics::get().draw(Segment(position, _pointOfIntersection), _isDetecting ? Color::Green : Color::Red);
		}
	}

	void Laser::update(float framePeriodInMillis)
	{
		const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		const Segment& segment = *(Segment*)EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_BOUNDS));
		Vector directionVector = segment.getNaturalVector().normalize();
		float movement = LASER_SPEED_PER_SECOND * framePeriodInMillis / 1000.0f;
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
		float maxMovement = Vector(maxPoint - position).getLength();
		if(maxMovement < movement)
		{
			movement = maxMovement;
			_isPositiveDirection = !_isPositiveDirection;
		}
		Point newPosition = position + directionVector * movement;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &newPosition));
		// Detect
	}
}