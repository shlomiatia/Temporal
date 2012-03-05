#pragma once

#include <vector>
#include "Entity.h"

namespace Temporal
{
	class ComponentOfTypeIteraor;

	class QueryManager
	{
	public:
		static QueryManager& get(void)
		{
			static QueryManager instance;
			return instance;
		}

		void dispose(void);

		void add(Entity* const entity);

		// TODO: Change to getters
		void sendMessageToAllEntities(Message& message) const;
		void sendMessageToEntity(int id, Message& message) const;
		const void* const sendQueryMessageToEntity(int id, Message& message) const;
	private:
		std::vector<Entity* const> _entities;

		QueryManager(void) {}
		QueryManager(const QueryManager&);
		QueryManager& operator=(const QueryManager&);
	};
}