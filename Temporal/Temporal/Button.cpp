#include "Button.h"

namespace Temporal
{
	const Hash Button::TYPE = Hash("button");

	void Button::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTIVATE)
		{
			if (_target != Hash::INVALID)
				getEntity().getManager().sendMessageToEntity(_target, Message(MessageID::ACTIVATE));
		}
	}
}