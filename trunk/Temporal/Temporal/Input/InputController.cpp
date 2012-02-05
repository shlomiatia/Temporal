#include "InputController.h"
#include "Input.h"

namespace Temporal
{
	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			if(Input::get().isUp())
			{
				sendMessageToOwner(Message(MessageID::ACTION_UP));
			}
			if(Input::get().isDown())
			{
				sendMessageToOwner(Message(MessageID::ACTION_DOWN));
			}
			if(Input::get().isLeft() || Input::get().isRight())
			{
				Orientation::Enum orientation =sendQueryMessageToOwner<Orientation::Enum>(MessageID::GET_ORIENTATION);
				if((Input::get().isLeft() && orientation == Orientation::LEFT) || (Input::get().isRight() && orientation == Orientation::RIGHT))
					sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
				else
					sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
			}
		}
	}
}