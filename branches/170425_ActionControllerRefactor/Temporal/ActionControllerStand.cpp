#include "ActionControllerStand.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "ActionController.h"
#include "JumpInfoByTypeProvider.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Stand::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void Stand::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::WALK_STATE);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::TURN_STATE);
			}
			else if (message.getID() == MessageID::ACTION_UP_START)
			{
				JumpInfoByTypeProvider helper(JumpType::UP);
				_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
			}
		}
	}
}
