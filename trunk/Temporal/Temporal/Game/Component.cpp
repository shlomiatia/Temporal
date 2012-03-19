#include "Component.h"
#include "Message.h"
#include "Entity.h"

namespace Temporal
{
	void* Component::sendMessageToOwner(Message& message) const
	{
		_entity->handleMessage(message);
		return message.getParam();
	}
}