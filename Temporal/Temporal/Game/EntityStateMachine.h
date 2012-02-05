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
		EntityStateMachine(EntityStateID::Enum initialState);
		~EntityStateMachine(void);

		virtual ComponentType::Enum getType(void) const { return ComponentType::STATE_MACHINE; }

		void changeState(EntityStateID::Enum state, const void* const param = NULL);

		virtual void handleMessage(Message& message);
	private:
		std::vector<EntityState*> _states;
		EntityState* _currentState;

		EntityStateMachine(const EntityStateMachine&) {};
		EntityStateMachine& operator=(const EntityStateMachine&) {};
	};
}

