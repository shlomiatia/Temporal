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
		ComponentOfTypeIteraor getComponentOfTypeIteraor(ComponentType::Type type);

		void broadcastMessage(Message& message);
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
		ComponentOfTypeIteraor(ComponentType::Type type) : _type(type), _current(NULL) { reset(); }

		void reset(void);
		Component& current(void) { return *_current; };
		bool next(void);
	private:
		ComponentType::Type _type;
		std::vector<Entity* const>::iterator _iterator;
		Component* _current;
	};
}