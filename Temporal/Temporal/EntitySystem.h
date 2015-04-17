#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include "GameState.h"
#include "GameEnums.h"
#include "Hash.h"
#include <unordered_map>
#include <vector>

namespace Temporal
{
	class Message
	{
	public:
		explicit Message(MessageID::Enum id, void* param = 0) : _id(id), _param(param) {}

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
		Component() : _entity(0) {}
		virtual ~Component() {}

		Entity& getEntity() { return *_entity; }
		const Entity& getEntity() const { return *_entity; }
		void init(Entity* entity) { _entity = entity; }
		virtual Hash getType() const = 0;

		virtual void handleMessage(Message& message) = 0;
		virtual Component* clone() const = 0;
		void* raiseMessage(Message& message) const;

	private:
		Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};

	typedef std::vector<Component*> ComponentCollection;
	typedef ComponentCollection::const_iterator ComponentIterator;

	class EntitiesManager;

	class Entity
	{
	public:
		Entity(Hash id = Hash::INVALID) : _id(id), _manager(0) {}
		~Entity();

		Hash getId() const { return _id; }
		void setId(Hash id) { _id = id; }
		EntitiesManager& getManager() { return *_manager; }

		void init(EntitiesManager* manager);
		void add(Component* component);
		const Component* get(Hash type) const;
		Component* get(Hash type);
		void* handleMessage(Message& message, const HashCollection* filter = 0) const;
		Entity* clone() const;
	private:
		Hash _id;
		ComponentCollection _components;

		EntitiesManager* _manager;

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, Entity*> HashEntityMap;
	typedef HashEntityMap::const_iterator EntityIterator;

	class EntitiesManager : public GameStateComponent
	{
	public:
		EntitiesManager() {};
		~EntitiesManager();

		void init(GameState* gameState);

		void sendMessageToAllEntities(Message& message, const HashCollection* filter = 0) const;
		void* sendMessageToEntity(Hash id, Message& message) const;
		HashEntityMap& getEntities() { return _entities; }
		Entity* getEntity(Hash id) const;
		void add(Entity* entity);
	private:
		HashEntityMap _entities;
		
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);

		friend class SerializationAccess;
	};
}

#endif