#pragma once

namespace Temporal { class EntityStateMachine; }

#include "Message.h"
#include "EntityStateMachine.h"
#include "MessageParams.h"

namespace Temporal
{
	class EntityState
	{
	public:
		EntityState(void) {};
		virtual ~EntityState(void) {};

		void setStateMachine(EntityStateMachine* stateMachine) { _stateMachine = stateMachine; }

		virtual const char* getName(void) const = 0;
		virtual void handleMessage(Message& message) = 0;

	protected:
		EntityStateMachine* _stateMachine;

		bool isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative = Direction::NONE) const;
		bool isSensorMessage(Message& message, SensorID::Enum sensorID) const;

	private:

		EntityState(const EntityState&);
		EntityState& operator=(const EntityState&);
	};
}