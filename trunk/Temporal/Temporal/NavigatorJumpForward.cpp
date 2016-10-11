#include "NavigatorJumpForward.h"
#include "MessageUtils.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash JUMP_FORWARD_STATE = Hash("NAV_STT_JUMP_FORWARD");
		static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");

		static const Hash ACTION_JUMP_STATE = Hash("ACT_STT_JUMP");
		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");
		static const Hash ACTION_WALK_STATE = Hash("ACT_STT_WALK");

		void JumpForward::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
			if (!_afterLoad)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_START));
			}

		}

		void JumpForward::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_ENTERED)
			{
				Hash state = getHashParam(message.getParam());
				if (state != ACTION_CLIMB_STATE && state != ACTION_JUMP_STATE && state != JUMP_FORWARD_STATE && state != ACTION_WALK_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
			}
		}
	}
}