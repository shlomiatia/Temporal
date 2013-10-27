#include "EntitySystem.h"
#include "Serialization.h"
#include "SerializationAccess.h"

#include "Keyboard.h"
#include "Input.h"
#include "Game.h"

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

	Component* Entity::get(Hash type) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			Component* component = *i;
			if(component->getType() == type)
				return component;
		}
		return 0;
	}
	
	void Entity::add(Component* component)
	{
		_components.push_back(component);
		component->init(this);
	}

	Entity* Entity::clone() const
	{
		Entity* clone = new Entity();
		clone->init(_manager);
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			clone->add((**i).clone());
		}
		return clone;
	}

	void* Entity::handleMessage(Message& message, const HashCollection* filter) const
	{
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			Component& component = **i;
			
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
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			delete (*i).second;
	}

	void EntitiesManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			i->second->init(this);
		sendMessageToAllEntities(Message(MessageID::ENTITY_PRE_INIT));
		sendMessageToAllEntities(Message(MessageID::ENTITY_INIT));
		sendMessageToAllEntities(Message(MessageID::ENTITY_POST_INIT));
		sendMessageToAllEntities(Message(MessageID::LEVEL_INIT));
	}

	void EntitiesManager::sendMessageToAllEntities(Message& message, const HashCollection* filter) const
	{
		// TODO:
		if(message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(key == Key::ESC || Input::get().getGamepad().getButton(GamepadButton::FRONT_RIGHT))
			{
				Game::get().stop();
			}
		}
		for(EntityIterator i = _entities.begin(); i != _entities.end(); ++i)
			(*(*i).second).handleMessage(message, filter);
	}

	void* EntitiesManager::sendMessageToEntity(Hash id, Message& message) const
	{
		return _entities.at(id)->handleMessage(message);
	}

	void EntitiesManager::add(Entity* entity)
	{
		_entities[entity->getId()] = entity;
		entity->init(this);
	}
}
