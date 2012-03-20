#include "InputController.h"
#include "Input.h"
#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageUtils.h>

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
			if(Input::get().isLeft())
			{
				sendDirectionAction(*this, Orientation::LEFT);
			}
			if(Input::get().isRight())
			{
				sendDirectionAction(*this, Orientation::RIGHT);
			}
		}
	}
}