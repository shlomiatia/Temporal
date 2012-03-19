#include "MessageUtils.h"
#include "Message.h"
#include "Component.h"

namespace Temporal
{
	void sendDirectionAction(const Component& component, Orientation::Enum direction)
	{
		Orientation::Enum orientation = *(Orientation::Enum*)component.sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		if(direction == orientation)
			component.sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
		else
			component.sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
	}
}