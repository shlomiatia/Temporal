#include "Component.h"

namespace Temporal
{
	void Component::sendMessageToOwner(Message& message) const
	{
		_entity->handleMessage(message);
	}
}