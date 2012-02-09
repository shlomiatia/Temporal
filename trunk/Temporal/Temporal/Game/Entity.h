#pragma once

namespace Temporal { class Component; }

#include <vector>
#include <Temporal\Base\Base.h>
#include "Message.h"
#include "Component.h"

namespace Temporal
{
	// TODO: ID & component order
	class Entity
	{
	public:
		Entity(void) {}
		~Entity(void);

		void add(Component* const component);
		Component* const getByType(ComponentType::Enum type) const;
		void handleMessage(Message& message) const;

	private:
		std::vector<Component* const> _components;
	};
}
