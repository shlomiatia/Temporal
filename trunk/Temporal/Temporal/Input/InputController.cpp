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
				sendMessage(Message(MessageID::ACTION_UP));
			}
			if(Input::get().isDown())
			{
				sendMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Input::get().isLeft() || Input::get().isRight())
			{
				Message getOrientation(MessageID::GET_ORIENTATION);
				sendMessage(getOrientation);
				Orientation::Type orientation = getOrientation.getParam<Orientation::Type>();
				if((Input::get().isLeft() && orientation == Orientation::LEFT) || (Input::get().isRight() && orientation == Orientation::RIGHT))
					sendMessage(Message(MessageID::ACTION_FORWARD));
				else
					sendMessage(Message(MessageID::ACTION_BACKWARD));
			}
		}
	}
}