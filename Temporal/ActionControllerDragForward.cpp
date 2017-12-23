#include "ActionControllerDragForward.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "ActionController.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void DragForward::enter(void* param)
		{
			// Frame flag 1 - Still walking forward
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION)));
			ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragForward::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_BACKWARDS_STATE);
			}
		}
	}
}
