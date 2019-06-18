#include "NavigatorJumpUp.h"
#include "MessageUtils.h"
#include "Navigator.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");

		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");

		void JumpUp::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
		}

		void JumpUp::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_CLIMB_STATE)
				{
					if (_afterLoad)
					{
						Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
						navigator.plotPath();
					}
					else
					{
						_stateMachine->changeState(WALK_STATE);
					}
				}
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_START));
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
			}
		}
	}
}