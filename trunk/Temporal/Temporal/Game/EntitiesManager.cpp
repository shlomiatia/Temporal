#include "EntitiesManager.h"
#include "Entity.h"
#include "Message.h"

namespace Temporal
{
	void EntitiesManager::dispose(void)
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			delete *i;
	}

	void EntitiesManager::add(Entity* entity)
	{
		_entities.push_back(entity);
		entity->handleMessage(Message(MessageID::ENTITY_CREATED));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message) const
	{
		sendMessageToAllEntities(message, ComponentType::ALL);
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message, ComponentType::Enum filter) const
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message, filter);
	}

	void* EntitiesManager::sendMessageToEntity(int id, Message& message) const
	{
		return _entities[id]->handleMessage(message);
	}

	void EntitiesManager::iterateEntities(ComponentType::Enum componentType, void* data, void (*handleEntity)(const Entity& entity, void* data))
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
		{
			const Entity& entity = **i;
			if(entity.have(componentType))
				handleEntity(**i, data);
		}
	}
}