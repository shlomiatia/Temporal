#include "Component.h"

namespace Temporal
{
	void Component::sendMessageToOwner(Message& message) const
	{
		_entity->handleMessage(message);
	}

	const void* const Component::sendQueryMessageToOwner(Message& query) const
	{
		sendMessageToOwner(query);
		return query.getParam();
	}
}