#include "Component.h"
#include "Message.h"
#include "Entity.h"

namespace Temporal
{
	const void* const Component::sendMessageToOwner(Message& message) const
	{
		_entity->handleMessage(message);
		return message.getParam();
	}
}