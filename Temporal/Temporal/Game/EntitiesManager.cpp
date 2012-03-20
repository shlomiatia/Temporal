#include "EntitiesManager.h"
#include "Entity.h"
#include "Message.h"

namespace Temporal
{
	void EntitiesManager::dispose(void)
	{
		for(std::vector<Entity*>::iterator i = _entities.begin(); i != _entities.end(); ++i)
			delete *i;
	}

	void EntitiesManager::add(Entity* entity)
	{
		_entities.push_back(entity);
		entity->handleMessage(Message(MessageID::ENTITY_CREATED));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message) const
	{
		for(std::vector<Entity*>::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message);
	}

	void* EntitiesManager::sendMessageToEntity(int id, Message& message) const
	{
		return _entities[id]->handleMessage(message);
	}

	void EntitiesManager::iterateEntities(ComponentType::Enum componentType, void* data, void (*handleEntity)(const Entity& entity, void* data))
	{
		for(std::vector<Entity*>::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
		{
			const Entity& entity = **i;
			if(entity.have(componentType))
				handleEntity(**i, data);
		}
	}
}