#include "MovingPlatform.h"
#include "MessageUtils.h"

namespace Temporal
{
	const Hash MovingPlatform::TYPE = Hash("moving-platform");

	static const float SPEED_PER_SECOND = 125.0f;

	void MovingPlatform::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTIVATE || message.getID() == MessageID::ENTITY_INIT || message.getID() == MessageID::BODY_COLLISION)
		{
			_movement *= -1.0f;
			raiseMessage(Message(MessageID::SET_IMPULSE, &_movement));
		}
	}
}