#include "Entity.h"
#include "Message.h"
#include "Component.h"

namespace Temporal
{
	Entity::~Entity()
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			delete *i;
	}

	void Entity::add(Component* component)
	{
		component->setEntity(this);
		_components.push_back(component);
	}

	const Component* Entity::get(ComponentType::Enum type) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			const Component* component = *i;
			if(component->getType() == type)
				return component;
		}
		return NULL;
	}

	void* Entity::handleMessage(Message& message, ComponentType::Enum filter) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			Component& component = **i;
			ComponentType::Enum type = component.getType();
			if(type & filter)
				component.handleMessage(message);
		}
		return message.getParam();
	}
}
