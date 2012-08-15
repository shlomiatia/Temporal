#include "EntitySystem.h"

namespace Temporal
{
	void* Component::raiseMessage(Message& message) const
	{
		_entity->handleMessage(message);
		return message.getParam();
	}

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

	Component* Entity::get(ComponentType::Enum type) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			Component* component = *i;
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

	void EntitiesManager::dispose()
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			delete (*i).second;
	}

	void EntitiesManager::add(const Hash& id, Entity* entity)
	{
		entity->setId(id);
		_entities[id] = entity;
		entity->handleMessage(Message(MessageID::ENTITY_CREATED));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message) const
	{
		sendMessageToAllEntities(message, ComponentType::ALL);
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message, ComponentType::Enum filter) const
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			(*(*i).second).handleMessage(message, filter);
	}

	void* EntitiesManager::sendMessageToEntity(const Hash& id, Message& message) const
	{
		return _entities.at(id)->handleMessage(message);
	}
}
