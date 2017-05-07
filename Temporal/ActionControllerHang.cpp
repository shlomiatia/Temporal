#include "ActionControllerHang.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Hang::enter(void* param)
		{
			bool bodyEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
			if (param)
				_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::HANG_ANIMATION)));
		}

		void Hang::exit(void* param)
		{
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
		}

		void Hang::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ACTION_DOWN)
			{
				_stateMachine->changeState(ActionControllerStateIds::FALL_STATE);
			}
			else if (message.getID() == MessageID::ACTION_UP_CONTINUE)
			{
				_stateMachine->changeState(ActionControllerStateIds::CLIMB_STATE);
			}
		}
	}
}
