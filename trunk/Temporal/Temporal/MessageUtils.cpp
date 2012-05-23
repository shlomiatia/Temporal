#include "MessageUtils.h"
#include "Message.h"
#include "Component.h"
#include "MessageParams.h"
#include "Hash.h"

namespace Temporal
{
	void sendDirectionAction(const Component& component, Side::Enum direction)
	{
		Side::Enum orientation = *(Side::Enum*)component.sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		if(direction == orientation)
			component.sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
		else
			component.sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
	}

	bool isSensorCollisionMessage(Message& message, const Hash& sensorID)
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			if(params.getSensorID() == sensorID)
				return true;
		}
		return false;
	}
}