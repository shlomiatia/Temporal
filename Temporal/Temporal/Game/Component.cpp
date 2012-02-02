#include "Component.h"

namespace Temporal
{
	void Component::sendMessage(Message& message) const
	{
		_entity->handleMessage(message);
	}
}