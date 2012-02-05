#include "Entity.h"

namespace Temporal
{
	Entity::~Entity(void)
	{
		for(std::vector<Component* const>::iterator i = _components.begin(); i != _components.end(); ++i)
			delete *i;
	}

	void Entity::add(Component* const component)
	{
		component->setEntity(this);
		_components.push_back(component);
	}

	Component* const Entity::getByType(ComponentType::Enum type)
	{
		for(std::vector<Component* const>::iterator i = _components.begin(); i != _components.end(); ++i)
			if((**i).getType() == type)
				return *i;
		return NULL;
	}

	void Entity::handleMessage(Message& message)
	{
		for(std::vector<Component* const>::iterator i = _components.begin(); i != _components.end(); ++i)
			(*i)->handleMessage(message);
	}
}
