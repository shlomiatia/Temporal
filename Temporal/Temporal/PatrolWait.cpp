#include "PatrolWait.h"
#include "Patrol.h"

namespace Temporal
{
	static const Hash TURN_STATE = Hash("PAT_STT_TURN");

	namespace PatrolStates
	{
		void Wait::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_HOLSTER));
		}

		void Wait::handleMessage(Message& message)
		{
			Patrol& patrol = *static_cast<Patrol*>(_stateMachine);

			if (patrol.handleTakedownMessage(message) ||
				patrol.handleFireMessage(message) ||
				patrol.handleAlarmMessage(message))
			{

			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (!patrol.isStatic() && _stateMachine->getTimer().getElapsedTime() >= patrol.getWaitTime())
				{
					_stateMachine->changeState(TURN_STATE);
				}
			}
		}

	}
}
