#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include "GameEnums.h"
#include "Hash.h"
#include <unordered_map>
#include <vector>
#include <stdio.h>

namespace Temporal
{
	class Message
	{
	public:
		explicit Message(MessageID::Enum id, void* param = NULL) : _id(id), _param(param) {}

		MessageID::Enum getID() const { return _id; }
		void* getParam() { return _param; }
		void setParam(void* param) { _param = param; }

	private:
		MessageID::Enum _id;
		void* _param;

		Message(const Message&);
		Message& operator=(const Message&);
	};

	class Entity;

	class Component
	{
	public:
		Component() : _entity(NULL) {}
		virtual ~Component() {}

		Entity& getEntity() const { return *_entity; }
		void setEntity(Entity* entity) { _entity = entity; }
		virtual ComponentType::Enum getType() const = 0;

		virtual void handleMessage(Message& message) = 0;
		virtual Component* clone() const { return NULL; }
		void* raiseMessage(Message& message) const;

	private:
		Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};

	typedef std::vector<Component*> ComponentCollection;
	typedef ComponentCollection::const_iterator ComponentIterator;

	class Entity
	{
	public:
		Entity() : _id(Hash::INVALID) {}
		~Entity();

		Hash getId() const { return _id; }
		void setId(Hash id) { _id = id; }
		void init();
		void add(Component* component);
		Component* get(ComponentType::Enum type) const;
		void* handleMessage(Message& message, ComponentType::Enum filter = ComponentType::ALL) const;
	private:
		Hash _id;
		ComponentCollection _components;

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, Entity*> EntityCollection;
	typedef EntityCollection::const_iterator EntityIterator;

	class EntitiesManager
	{
	public:
		static EntitiesManager& get()
		{
			static EntitiesManager instance;
			return instance;
		}

		void init();
		void dispose();
		void add(Entity* entity);

		void sendMessageToAllEntities(Message& message) const;
		void sendMessageToAllEntities(Message& message, ComponentType::Enum filter) const;
		void* sendMessageToEntity(Hash id, Message& message) const;
		const EntityCollection& getEntities() const { return _entities; }
	private:
		EntityCollection _entities;

		EntitiesManager() {}
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);
	};
}

#endif