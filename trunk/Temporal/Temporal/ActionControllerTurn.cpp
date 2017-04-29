#include "ActionControllerTurn.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Turn::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::TURN_ANIMATION)));
		}

		void Turn::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
				_stateMachine->changeState(ActionControllerStateIds::WALK_STATE);
			}
		}
	}
}
