#include "EntitiesManager.h"
#include "Entity.h"
#include "Message.h"

namespace Temporal
{
	void EntitiesManager::dispose(void)
	{
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			delete (*i).second;
	}

	void EntitiesManager::add(const Hash& id, const Entity* entity)
	{
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