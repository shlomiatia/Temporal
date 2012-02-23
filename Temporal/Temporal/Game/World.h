#pragma once

#include <vector>
#include "Entity.h"

namespace Temporal
{
	class ComponentOfTypeIteraor;

	class World
	{
	public:
		static World& get(void)
		{
			static World instance;
			return (instance);
		}

		void dispose(void);

		void add(Entity* const entity);

		ComponentOfTypeIteraor getComponentOfTypeIteraor(ComponentType::Enum type) const;
		void sendMessageToAllEntities(Message& message) const;
		void sendMessageToEntity(int id, Message& message) const;
		const void* const sendQueryMessageToEntity(int id, Message& message) const;
	private:
		std::vector<Entity* const> _entities;

		World(void) {}
		World(const World&);
		World& operator=(const World&);

		friend class ComponentOfTypeIteraor;
	};

	class ComponentOfTypeIteraor
	{
	public:
		ComponentOfTypeIteraor(ComponentType::Enum type) : _type(type), _current(NULL) { reset(); }

		void reset(void);
		Component& current(void) { return *_current; };
		bool next(void);
	private:
		ComponentType::Enum _type;
		std::vector<Entity* const>::iterator _iterator;
		Component* _current;
	};
}