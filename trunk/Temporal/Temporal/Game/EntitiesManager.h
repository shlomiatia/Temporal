#pragma once

#include <vector>
#include "Entity.h"

namespace Temporal
{
	class ComponentOfTypeIteraor;

	class EntitiesManager
	{
	public:
		static EntitiesManager& get(void)
		{
			static EntitiesManager instance;
			return instance;
		}

		void dispose(void);

		void add(Entity* const entity);

		void sendMessageToAllEntities(Message& message) const;
		const void* const sendMessageToEntity(int id, Message& message) const;
	private:
		std::vector<Entity* const> _entities;

		EntitiesManager(void) {}
		EntitiesManager(const EntitiesManager&);
		EntitiesManager& operator=(const EntitiesManager&);
	};
}