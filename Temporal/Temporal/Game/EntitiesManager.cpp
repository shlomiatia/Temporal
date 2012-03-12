#include "EntitiesManager.h"
#include "Entity.h"
#include "Message.h"

namespace Temporal
{
	void EntitiesManager::dispose(void)
	{
		for(std::vector<Entity* const>::iterator i = _entities.begin(); i != _entities.end(); ++i)
			delete *i;
	}

	void EntitiesManager::add(Entity* entity)
	{
		_entities.push_back(entity);
		entity->handleMessage(Message(MessageID::ENTITY_CREATED));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message) const
	{
		for(std::vector<Entity* const>::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message);
	}

	const void* const EntitiesManager::sendMessageToEntity(int id, Message& message) const
	{
		_entities[id]->handleMessage(message);
		return message.getParam();
	}
}