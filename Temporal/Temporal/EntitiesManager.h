#ifndef ENTITIESMANAGER_H
#define ENTITIESMANAGER_H

#include "GameEnums.h"
#include "Hash.h"
#include <unordered_map>

namespace Temporal
{
	class Entity;
	class Message;

	typedef std::unordered_map<const Hash, const Entity*> EntityCollection;
	typedef EntityCollection::const_iterator EntityIterator;

	class EntitiesManager
	{
	public:
		static EntitiesManager& get()
		{
			static EntitiesManager instance;
			return instance;
		}

		void dispose();

		void add(const Hash& id, const Entity* entity);

		void sendMessageToAllEntities(Message& message) const;
		void sendMessageToAllEntities(Message& message, ComponentType::Enum filter) const;
		void* sendMessageToEntity(const Hash& id, Message& message) const;
		const EntityCollection& getEntities() const { return _entities; }
	private:
		EntityCollection _entities;

		EntitiesManager() {}
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);
	};
}
#endif