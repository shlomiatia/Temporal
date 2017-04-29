#include "ActionControllerJumpEnd.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void JumpEnd::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void JumpEnd::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				// BRODER
				if (_stateMachine->getTimer().getElapsedTime() > 0.1f)
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
