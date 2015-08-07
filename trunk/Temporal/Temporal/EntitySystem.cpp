#include "EntitySystem.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Vector.h"

#include <algorithm> 

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

	void Entity::init(EntitiesManager* manager)
	{
		_manager = manager;
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
			(**i).init(this);
	}

	const Component* Entity::get(Hash type) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			Component* component = *i;
			if(component->getType() == type)
				return component;
		}
		return 0;
	}

	Component* Entity::get(Hash type)
	{
		return const_cast<Component*>(const_cast<const Entity*>(this)->get(type));
	}
	
	void Entity::add(Component* component)
	{
		_components.push_back(component);
		component->init(this);
		if (_manager)
		{
			component->handleMessage(Message(MessageID::ENTITY_PRE_INIT));
			component->handleMessage(Message(MessageID::ENTITY_INIT));
			component->handleMessage(Message(MessageID::ENTITY_POST_INIT));
		}
		
	}

	Entity* Entity::clone() const
	{
		Entity* clone = new Entity();
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			clone->add((**i).clone());
		}
		clone->init(_manager);
		return clone;
	}

	void* Entity::handleMessage(Message& message, const HashList* filter) const
	{
		for (int i = 0; i < _components.size(); ++i)
		{
			Component& component = *_components[i];
			
			if(filter != 0)
			{
				bool isMatch = false;
				Hash type = component.getType();
				for(HashIterator j = filter->begin(); j != filter->end(); ++j)
				{
					if(*j == type)
					{
						isMatch = true;
						break;
					}
				}
				if(!isMatch)
					continue;
			}
			component.handleMessage(message);
		}
		return message.getParam();
	}

	EntitiesManager::~EntitiesManager()
	{
		sendMessageToAllEntities(Message(MessageID::LEVEL_DISPOSED));
		sendMessageToAllEntities(Message(MessageID::ENTITY_DISPOSED));
		for(HashEntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			delete (*i).second;
	}

	void EntitiesManager::init(GameState* gameState)
	{
		_initializing = true;
		GameStateComponent::init(gameState);
		for(HashEntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			i->second->init(this);
		sendMessageToAllEntities(Message(MessageID::ENTITY_PRE_INIT));
		sendMessageToAllEntities(Message(MessageID::ENTITY_INIT));
		sendMessageToAllEntities(Message(MessageID::ENTITY_POST_INIT));
		_initializing = false;
		sendMessageToAllEntities(Message(MessageID::LEVEL_INIT));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message, const HashList* filter) const
	{
		for(HashEntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			(*(*i).second).handleMessage(message, filter);
	}

	void* EntitiesManager::sendMessageToEntity(Hash id, Message& message) const
	{
		return _entities.at(id)->handleMessage(message);
	}

	Entity* EntitiesManager::getEntity(Hash id) const
	{
		HashEntityIterator i = _entities.find(id);
		if(i == _entities.end())
			return 0;
		return i->second;
	}

	void EntitiesManager::add(Entity* entity)
	{
		if (_entities.find(entity->getId()) != _entities.end() || _initializing)
			abort();
		_entities[entity->getId()] = entity;
		entity->init(this);
		entity->handleMessage(Message(MessageID::ENTITY_PRE_INIT));
		entity->handleMessage(Message(MessageID::ENTITY_INIT));
		entity->handleMessage(Message(MessageID::ENTITY_POST_INIT));
	}

	void EntitiesManager::remove(Hash id)
	{
		HashEntityIterator i = _entities.find(id);
		Entity* entity = i->second;
		entity->handleMessage(Message(MessageID::ENTITY_DISPOSED));
		_entities.erase(i);
		delete entity;
	}

	void EntitiesManager::removeInputComponent(Component* component)
	{
		_inputComponents.erase(std::remove(_inputComponents.begin(), _inputComponents.end(), component), _inputComponents.end());
	}

	EntityTemplatesManager::~EntityTemplatesManager()
	{
		for (HashEntityIterator i = _templates.begin(); i != _templates.end(); ++i)
			delete (*i).second;
	}

	void EntityTemplatesManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);
		XmlDeserializer deserializer(new FileStream("resources/game-states/templates.xml", false, false));
		deserializer.serialize("entity", _templates);
		_templateIterator = _templates.begin();
	}

	Entity* EntityTemplatesManager::cloneByTemplateId(Hash templateId)
	{
		Entity* newEntity = _templates.at(templateId)->clone();
		return newEntity;
	}

	void EntityTemplatesManager::previousTemplate()
	{
		if (_templateIterator == _templates.begin())
		{
			_templateIterator = _templates.end();
		}
		--_templateIterator;
	}

	void EntityTemplatesManager::nextTemplate()
	{
		++_templateIterator;
		if (_templateIterator == _templates.end())
			_templateIterator = _templates.begin();
	}

	
}
