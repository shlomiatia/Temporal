#include "PatrolFire.h"

namespace Temporal
{
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");

	namespace PatrolStates
	{
		void Fire::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::STOP_NAVIGATE));
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FIRE));
		}

		void Fire::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
		}

	}
}
