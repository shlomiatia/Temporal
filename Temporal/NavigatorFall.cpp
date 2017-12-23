#include "NavigatorFall.h"
#include "MessageUtils.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");

		static const Hash ACTION_JUMP_END_STATE = Hash("ACT_STT_JUMP_END");

		void Fall::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
		}

		void Fall::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_JUMP_END_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}
	}
}