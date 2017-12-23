#include "NavigatorSkipBox.h"
#include "MessageUtils.h"
#include "Navigator.h"

namespace Temporal
{
	static const Hash ACTION_FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");

	namespace NavigatorStates
	{
		void SkipBox::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_CLIMB_STATE)
				{
					_stateMachine->setStateFlag(true);
				}
				else if (state == ACTION_FALL_STATE)
				{
					Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
					navigator.plotPath();
				}
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (_stateMachine->getStateFlag())
				{
					_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
				}
				else
				{
					_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_START));
					_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
				}

			}
		}
	}
}