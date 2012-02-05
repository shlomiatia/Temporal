#include "World.h"

namespace Temporal
{
	void World::dispose(void)
	{
		for(std::vector<Entity* const>::iterator i = _entities.begin(); i != _entities.end(); ++i)
			delete *i;
	}

	void World::add(Entity* const entity)
	{
		_entities.push_back(entity);
	}

	ComponentOfTypeIteraor World::getComponentOfTypeIteraor( ComponentType::Enum type )
	{
		return ComponentOfTypeIteraor(type);
	}

	void World::sendMessageToAllEntities(Message& message)
	{
		for(std::vector<Entity* const>::iterator i = _entities.begin(); i != _entities.end(); ++i)
			(**i).handleMessage(message);
	}

	void ComponentOfTypeIteraor::reset(void)
	{
		_iterator = World::get()._entities.begin();
		_current = NULL;
	}

	bool ComponentOfTypeIteraor::next(void)
	{
		if(_iterator == World::get()._entities.end())	
			return false;
		do 
		{
			_current = (**_iterator).getByType(_type);
			++_iterator;
		} 
		while (_current == NULL && _iterator != World::get()._entities.end());
		return _current != NULL;
	}
}