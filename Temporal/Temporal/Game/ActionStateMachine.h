#pragma once

namespace Temporal { class ActionState; }

#include <vector>
#include "Enums.h"
#include "ActionState.h"
#include "Component.h"

namespace Temporal
{
	// TODO: Extract similar data for state machines SLOTH!
	class ActionStateMachine : public Component
	{
	public:
		// TODO: Where to put movement constants
		static const float WALK_FORCE_PER_SECOND;
		static const float JUMP_FORCE_PER_SECOND;

		ActionStateMachine(void);
		~ActionStateMachine(void);

		virtual ComponentType::Enum getType(void) const { return ComponentType::ACTION_STATE_MACHINE; }

		void changeState(ActionStateID::Enum state, const void* const param = NULL);

		virtual void handleMessage(Message& message);
	private:
		std::vector<ActionState*> _states;
		ActionState* _currentState;
		
		ActionStateMachine(const ActionStateMachine&);
		ActionStateMachine& operator=(const ActionStateMachine&);
	};
}

