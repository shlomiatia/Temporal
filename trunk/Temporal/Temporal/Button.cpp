#include "Button.h"

namespace Temporal
{
	const Hash Button::TYPE = Hash("button");

	void Button::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTIVATE)
		{
			if (_target != Hash::INVALID)
				getEntity().getManager().sendMessageToEntity(_target, message);
		}
		else if (message.getID() == MessageID::IS_ACTIVATED)
		{
			if (_target != Hash::INVALID)
				getEntity().getManager().sendMessageToEntity(_target, message);
		}
	}
}