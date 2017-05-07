#include "ActionControllerFire.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		static const Hash PLAYER("ENT_PLAYER");

		void Fire::enter(void* param)
		{
			float weight = getFloatParam(param);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FIRE_DOWN_ANIMATION, false, 0.0f, 1)));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FIRE_UP_ANIMATION, false, 0.0f, 2, weight)));
			_stateMachine->getEntity().getManager().sendMessageToEntity(Hash(PLAYER), Message(MessageID::DIE));
		}

		void Fire::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
