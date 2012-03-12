#pragma once

#include "Enums.h"
#include <vector>

namespace Temporal
{
	class Component;
	class Message;

	// TODO: ID & component order ENTITIES
	class Entity
	{
	public:
		~Entity(void);

		void add(Component* const component);
		Component* const getByType(ComponentType::Enum type) const;
		void handleMessage(Message& message) const;

	private:
		std::vector<Component* const> _components;
	};
}
