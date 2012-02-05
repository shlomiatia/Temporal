#pragma once

namespace Temporal { class EntityState; }

#include <vector>
#include "Enums.h"
#include "EntityState.h"
#include "Component.h"

namespace Temporal
{
	class EntityStateMachine : public Component
	{
	public:
		EntityStateMachine(EntityStateID::Type initialState);
		~EntityStateMachine(void);

		virtual ComponentType::Type getType(void) const { return ComponentType::STATE_MACHINE; }

		void changeState(EntityStateID::Type state, const void* const param = NULL);

		virtual void handleMessage(Message& message);
	private:
		std::vector<EntityState*> _states;
		EntityState* _currentState;

		EntityStateMachine(const EntityStateMachine&) {};
		EntityStateMachine& operator=(const EntityStateMachine&) {};

		friend class EntityState;
	};
}

