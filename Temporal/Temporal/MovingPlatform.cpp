#include "MovingPlatform.h"
#include "MessageUtils.h"

namespace Temporal
{
	const Hash MovingPlatform::TYPE = Hash("moving-platform");

	static const float SPEED_PER_SECOND = 125.0f;

	void MovingPlatform::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND, 0.0f)));
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			float modifier = collision.getX() < 0.0f ? 1.0f : -1.0f;
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND * modifier, 0.0f)));
		}
	}
}