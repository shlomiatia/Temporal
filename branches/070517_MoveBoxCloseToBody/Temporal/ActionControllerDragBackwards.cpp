#include "ActionControllerDragBackwards.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "ActionController.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void DragBackwards::enter(void* param)
		{
			// Frame flag 1 - Still walking backwards
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION, true)));
			ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragBackwards::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_FORWARD_STATE);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
		}
	}
}
