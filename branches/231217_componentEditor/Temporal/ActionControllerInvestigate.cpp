#include "ActionControllerInvestigate.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Investigate::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::INVESTIGATE_ANIMATION)));
		}

		void Investigate::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
