#pragma once

namespace Temporal { class AIState; }

#include <vector>
#include "AIState.h"
#include <Temporal/Game/Enums.h>
#include <Temporal/Game/Component.h>

namespace Temporal
{
	class AIStateMachine : public Component
	{
	public:
		AIStateMachine(std::vector<AIState*> states);
		virtual ~AIStateMachine(void);

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }
		virtual void handleMessage(Message& message);

		void changeState(int state);

	protected:
		virtual int getInitialState(void) const = 0;

	private:
		std::vector<AIState*> _states;
		AIState* _currentState;

		AIStateMachine(const AIStateMachine&);
		AIStateMachine& operator=(const AIStateMachine&);
	};
}

