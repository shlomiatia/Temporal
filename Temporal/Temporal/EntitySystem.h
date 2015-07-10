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
		Component(bool bypassSave = false) : _entity(0), _bypassSave(bypassSave){}
		virtual ~Component() {}

		Entity& getEntity() { return *_entity; }
		const Entity& getEntity() const { return *_entity; }
		void init(Entity* entity) { _entity = entity; }

		bool isBypassSave() const { return _bypassSave; }
		void setBypassSave(bool bypassSave) { _bypassSave = bypassSave; }

		virtual Hash getType() const = 0;
		virtual void handleMessage(Message& message) = 0;
		virtual Component* clone() const = 0;
		void* raiseMessage(Message& message) const;

	private:
		Entity* _entity;
		bool _bypassSave;

		Component(const Component&);
		Component& operator=(const Component&);
	};

	typedef std::vector<Component*> ComponentList;
	typedef ComponentList::const_iterator ComponentIterator;

	class EntitiesManager;

	class Entity
	{
	public:
		Entity(Hash id = Hash::INVALID) : _id(id), _manager(0), _bypassSave(false) {}
		~Entity();

		Hash getId() const { return _id; }
		void setId(Hash id) { _id = id; }
		EntitiesManager& getManager() { return *_manager; }

		void init(EntitiesManager* manager);
		void add(Component* component);
		const Component* get(Hash type) const;
		Component* get(Hash type);
		void* handleMessage(Message& message, const HashList* filter = 0) const;
		Entity* clone() const;

		bool isBypassSave() const { return _bypassSave; }
		void setBypassSave(bool bypassSave) { _bypassSave = bypassSave; }
	private:
		Hash _id;
		ComponentList _components;
		bool _bypassSave;

		EntitiesManager* _manager;

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, Entity*> HashEntityMap;
	typedef HashEntityMap::const_iterator HashEntityIterator;

	class EntitiesManager : public GameStateComponent
	{
	public:
		EntitiesManager() : _focusInputComponent(0), _initializing(false) {};
		~EntitiesManager();

		void init(GameState* gameState);

		void sendMessageToAllEntities(Message& message, const HashList* filter = 0) const;
		void* sendMessageToEntity(Hash id, Message& message) const;
		HashEntityMap& getEntities() { return _entities; }
		Entity* getEntity(Hash id) const;
		void add(Entity* entity);
		void remove(Hash id);

		ComponentList& getInputComponents() { return _inputComponents; }
		void addInputComponent(Component* component) { _inputComponents.push_back(component); }
		void setFocusInputComponent(Component* component) { _focusInputComponent = component; }
		void clearFocusInputComponent() { _focusInputComponent = 0; }
		Component* getFocusInputComponent() const { return _focusInputComponent; }
	private:
		HashEntityMap _entities;
		ComponentList _inputComponents;
		Component* _focusInputComponent;
		bool _initializing;
		
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);

		friend class SerializationAccess;
	};

	class Vector;

	class EntityTemplatesManager : public GameStateComponent
	{
	public:
		~EntityTemplatesManager();

		void init(GameState* gameState);

		Hash getCurrentTemplateId() const { return  _templateIterator->first; }
		Entity* cloneCurrent(Hash entityId, const Vector& position) { return cloneByTemplateId(_templateIterator->first, entityId, position); };
		void previousTemplate();
		void nextTemplate();

	private:
		HashEntityMap _templates;
		HashEntityIterator _templateIterator;

		Entity* cloneByTemplateId(Hash templateId, Hash entityId, const Vector& position);
		
		friend class SerializationAccess;
	};
}

#endif