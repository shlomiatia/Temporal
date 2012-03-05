#include "QueryManager.h"

namespace Temporal
{
	void QueryManager::dispose(void)
	{
		for(std::vector<Entity* const>::iterator i = _entities.begin(); i != _entities.end(); ++i)
			delete *i;
	}

	void QueryManager::add(Entity* const entity)
	{
		_entities.push_back(entity);
		entity->handleMessage(Message(MessageID::ENTITY_CREATED));
	}

	void QueryManager::sendMessageToAllEntities(Message& message) const
	{
		for(std::vector<Entity* const>::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message);
	}

	const void* const QueryManager::sendMessageToEntity(int id, Message& message) const
	{
		_entities[id]->handleMessage(message);
		return message.getParam();
	}
}