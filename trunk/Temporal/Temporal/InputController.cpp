#include "InputController.h"
#include "Input.h"
#include "BaseEnums.h"
#include "MessageUtils.h"

namespace Temporal
{
	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			if(Input::get().isUp())
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(Input::get().isDown())
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Input::get().isLeft())
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(Input::get().isRight())
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
		}
	}
}