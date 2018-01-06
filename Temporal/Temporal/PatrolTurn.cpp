#include "PatrolTurn.h"
#include "CommonMessagesHandler.h"

namespace Temporal
{
	static const Hash WALK_STATE = Hash("PAT_STT_WALK");

	static const Hash ACTION_TURN_STATE = Hash("ACT_STT_TURN");

	namespace PatrolStates
	{
		void Turn::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_BACKWARD));
		}

		void Turn::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash actionID = getHashParam(message.getParam());
				if (actionID == ACTION_TURN_STATE)
				{
					_stateMachine->changeState(WALK_STATE);
				}
			}
		}
	}
}
