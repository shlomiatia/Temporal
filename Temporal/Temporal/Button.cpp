#include "Button.h"

namespace Temporal
{
	void Button::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTIVATE)
		{
			EntitiesManager::get().sendMessageToEntity(_target, Message(MessageID::ACTIVATE));
		}
	}
}