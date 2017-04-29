#include "ActionControllerFall.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "ActionController.h"
#include "JumpInfoByTypeProvider.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Fall::enter(void* param)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FALL_ANIMATION)));
		}

		void Fall::handleMessage(Message& message)
		{
			// TempFlag 1 - want to hang
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_UP_START)
			{
				// Need to be in start jump start, because can occur on fall from ledge
				if (_stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
				{
					JumpInfoByTypeProvider helper(JumpType::FORWARD);
					_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
				}
			}
		}
	}
}
