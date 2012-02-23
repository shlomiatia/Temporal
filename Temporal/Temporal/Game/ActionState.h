#pragma once

namespace Temporal { class ActionStateMachine; }

#include "Message.h"
#include "ActionStateMachine.h"
#include "MessageParams.h"

namespace Temporal
{
	class ActionState
	{
	public:
		ActionState(void) {};
		virtual ~ActionState(void) {};

		void setStateMachine(ActionStateMachine* stateMachine) { _stateMachine = stateMachine; }

		virtual const char* getName(void) const = 0;
		virtual void handleMessage(Message& message) = 0;

	protected:
		ActionStateMachine* _stateMachine;

		bool isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative = Direction::NONE) const;
		bool isSensorMessage(Message& message, SensorID::Enum sensorID) const;

	private:

		ActionState(const ActionState&);
		ActionState& operator=(const ActionState&);
	};
}