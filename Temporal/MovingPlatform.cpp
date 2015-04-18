#include "MovingPlatform.h"
#include "MessageUtils.h"
#include "Grid.h"
#include "Math.h"

namespace Temporal
{
	const Hash MovingPlatform::TYPE = Hash("moving-platform");

	static const float SPEED_PER_SECOND = 50.0f;

	void MovingPlatform::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());			
			const Vector& position = getPosition(*this);
			const Vector& destination = _direction ? _point1 : _point2;
			Vector distance = destination - position;
			Vector direction = distance.normalize();
			Vector movement = direction * SPEED_PER_SECOND * framePeriod;
			Vector newPosition = position + movement;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			
			Vector nextDistance = destination - newPosition;
			if(differentSign(distance.getX(), nextDistance.getX()) || differentSign(distance.getY(), nextDistance.getY()))
				_direction = !_direction;
		}
	}
}