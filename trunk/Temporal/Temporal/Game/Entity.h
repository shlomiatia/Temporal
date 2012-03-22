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

	class Entity
	{
	public:
		~Entity(void);

		void add(Component* component);
		bool have(ComponentType::Enum type) const;
		void* handleMessage(Message& message, ComponentType::Enum filter = ComponentType::ALL) const;

	private:
		ComponentCollection _components;
	};
}

#endif