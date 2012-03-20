#include "Entity.h"
#include "Message.h"
#include "Component.h"

namespace Temporal
{
	Entity::~Entity(void)
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			delete *i;
	}

	void Entity::add(Component* component)
	{
		component->setEntity(this);
		_components.push_back(component);
	}

	bool Entity::have(ComponentType::Enum type) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			if((**i).getType() == type)
				return true;
		return false;
	}

	void* Entity::handleMessage(Message& message) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			(**i).handleMessage(message);
		return message.getParam();
	}
}
