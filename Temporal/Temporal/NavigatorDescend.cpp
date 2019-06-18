#include "NavigatorDescend.h"
#include "Navigator.h"
#include "MessageUtils.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");

		static const Hash ACTION_JUMP_END_STATE = Hash("ACT_STT_JUMP_END");

		void Descend::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
		}

		void Descend::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_JUMP_END_STATE)
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
				_stateMachine->raiseMessage(Message(MessageID::ACTION_DOWN));
			}
		}
	}
}