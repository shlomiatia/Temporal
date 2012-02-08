#pragma once

namespace Temporal { class EntityState; }

#include <vector>
#include "Enums.h"
#include "EntityState.h"
#include "Component.h"

namespace Temporal
{
	// TODO: Extract state machine
	class EntityStateMachine : public Component
	{
	public:
		// TODO: Where to put movement constants
		static const float WALK_FORCE;
		static const float JUMP_FORCE;

		// TODO: Where to get initial state
		EntityStateMachine(EntityStateID::Enum initialState);
		~EntityStateMachine(void);

		virtual ComponentType::Enum getType(void) const { return ComponentType::STATE_MACHINE; }
		void resetDrawPositionOverride(void) { _drawPositionOverride = Vector::Zero; }
		void setDrawPositionOverride(Vector val) { _drawPositionOverride = val; }

		// TODO: Change state with a message
		void changeState(EntityStateID::Enum state, const void* const param = NULL);

		virtual void handleMessage(Message& message);
	private:
		std::vector<EntityState*> _states;
		EntityState* _currentState;
		Vector _drawPositionOverride;
		
		EntityStateMachine(const EntityStateMachine&);
		EntityStateMachine& operator=(const EntityStateMachine&);
	};
}

