#include "Door.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	const Hash Door::TYPE = Hash("door");

	void Door::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			set();
		}
		else if (message.getID() == MessageID::ACTIVATE)
		{
			_closed = !_closed;
			set();
		}
	}

	void Door::set()
	{
		int collisionCategory = _closed ? CollisionCategory::OBSTACLE : -1;
		raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &collisionCategory));
		float alpha = _closed ? 1.0f : 0.0f;
		raiseMessage(Message(MessageID::SET_ALPHA, &alpha));
	}
}