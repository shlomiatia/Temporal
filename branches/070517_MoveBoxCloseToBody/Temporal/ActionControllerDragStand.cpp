#include "ActionControllerDragStand.h"
#include "ActionController.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void DragStand::enter(void* param)
		{
			if (param)
			{
				ActionController::getActionController(_stateMachine).getDragMessagesHandler().setDraggableId(getHashParam(param));
			}
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
			ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragEnter();
		}

		void DragStand::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getDragMessagesHandler().handleDragMessage(message))
			{

			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_FORWARD_STATE);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_BACKWARDS_STATE);
			}
		}
	}
}
