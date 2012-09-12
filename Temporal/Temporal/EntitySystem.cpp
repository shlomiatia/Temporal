#include "EntitySystem.h"
#include "Serialization.h"
#include "SerializationAccess.h"

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

	void Entity::init()
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			(**i).setEntity(this);
	}

	void Entity::add(Component* component)
	{
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

	void EntitiesManager::init()
	{
		XmlDeserializer deserializer("entities.xml");
		deserializer.serialize("entity", _entities);
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			i->second->init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::ENTITY_PRE_INIT));
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::ENTITY_INIT));
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::ENTITY_POST_INIT));
	}

	void EntitiesManager::add(Entity* entity)
	{
		_entities[entity->getId()] = entity;
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

	void* EntitiesManager::sendMessageToEntity(Hash id, Message& message) const
	{
		return _entities.at(id)->handleMessage(message);
	}
}
