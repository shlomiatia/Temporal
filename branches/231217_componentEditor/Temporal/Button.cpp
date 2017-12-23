#include "Button.h"

namespace Temporal
{
	void Button::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::TOGGLE_ACTIVATION)
		{
			if (_target != Hash::INVALID)
				getEntity().getManager().sendMessageToEntity(_target, message);
		}
	}
}