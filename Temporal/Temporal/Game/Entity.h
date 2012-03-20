#ifndef ENTITY_H
#define ENTITY_H

#include "GameEnums.h"
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

		void add(Component* component);
		Component* getByType(ComponentType::Enum type) const;
		void handleMessage(Message& message) const;

	private:
		std::vector<Component*> _components;
	};
}

#endif