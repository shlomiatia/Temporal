#ifndef ENTITY_H
#define ENTITY_H

#include "GameEnums.h"
#include <vector>

namespace Temporal
{
	class Component;
	class Message;

	typedef std::vector<Component*> ComponentCollection;
	typedef ComponentCollection::const_iterator ComponentIterator;

	// TODO: ID & component order ENTITIES
	class Entity
	{
	public:
		~Entity(void);

		void add(Component* component);
		bool have(ComponentType::Enum type) const;
		void* handleMessage(Message& message) const;

	private:
		ComponentCollection _components;
	};
}

#endif