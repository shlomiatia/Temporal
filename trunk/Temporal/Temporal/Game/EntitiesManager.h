#ifndef ENTITIESMANAGER_H
#define ENTITIESMANAGER_H

#include "GameEnums.h"
#include <vector>

namespace Temporal
{
	class Entity;
	class Message;

	class EntitiesManager
	{
	public:
		static EntitiesManager& get(void)
		{
			static EntitiesManager instance;
			return instance;
		}

		void dispose(void);

		void add(Entity* entity);

		void sendMessageToAllEntities(Message& message) const;
		void* sendMessageToEntity(int id, Message& message) const;
		void iterateEntities(ComponentType::Enum componentType, void* data, void (*handleEntity)(const Entity& entity, void* data));
	private:
		std::vector<Entity*> _entities;

		EntitiesManager(void) {}
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);
	};
}
#endif