#include "Button.h"

namespace Temporal
{
	void Button::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTIVATE)
		{
			getEntity().getManager().sendMessageToEntity(_target, Message(MessageID::ACTIVATE));
		}
	}
}