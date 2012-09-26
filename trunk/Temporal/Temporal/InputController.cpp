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
			if(Input::get().key(Key::LEFT))
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(Input::get().key(Key::RIGHT))
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
			if(Input::get().key(Key::UP))
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(Input::get().key(Key::DOWN))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
		}
	}
}