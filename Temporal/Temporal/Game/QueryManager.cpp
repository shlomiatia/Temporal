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

	ComponentOfTypeIteraor QueryManager::getComponentOfTypeIteraor(ComponentType::Enum type) const
	{
		return ComponentOfTypeIteraor(type);
	}

	void QueryManager::sendMessageToAllEntities(Message& message) const
	{
		for(std::vector<Entity* const>::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message);
	}

	void QueryManager::sendMessageToEntity(int id, Message& message) const
	{
		_entities[id]->handleMessage(message);
	}

	const void* const QueryManager::sendQueryMessageToEntity(int id, Message& query) const
	{
		sendMessageToEntity(id, query);
		return query.getParam();
	}

	void ComponentOfTypeIteraor::reset(void)
	{
		_iterator = QueryManager::get()._entities.begin();
		_current = NULL;
	}

	bool ComponentOfTypeIteraor::next(void)
	{
		if(_iterator == QueryManager::get()._entities.end())	
			return false;
		do 
		{
			_current = (**_iterator).getByType(_type);
			++_iterator;
		} 
		while (_current == NULL && _iterator != QueryManager::get()._entities.end());
		return _current != NULL;
	}
}