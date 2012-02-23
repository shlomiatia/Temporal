#pragma once

namespace Temporal { class AIStateMachine; }

#include "AIStateMachine.h"
#include <Temporal/Game/Message.h>

namespace Temporal
{
	class AIState
	{
	public:
		AIState(void) {};
		virtual ~AIState(void) {};

		void setStateMachine(AIStateMachine* stateMachine) { _stateMachine = stateMachine; }

		virtual void handleMessage(Message& message) = 0;

	protected:
		AIStateMachine* _stateMachine;

	private:
		AIState(const AIState&);
		AIState& operator=(const AIState&);
	};
}