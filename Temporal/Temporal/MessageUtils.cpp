#include "MessageUtils.h"
#include "Message.h"
#include "Component.h"
#include "MessageParams.h"
#include "Hash.h"
#include "EntitiesManager.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

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

	int getPeriod(int* periodPointer)
	{
		int period = periodPointer == NULL ? 0 : *periodPointer;
		return period;
	}

	int getPeriod(const Component& component)
	{
		int* periodPointer = (int*)component.sendMessageToOwner(Message(MessageID::GET_PERIOD));
		return getPeriod(periodPointer);
	}

	int getPlayerPeriod(void)
	{
		int* periodPointer = (int*)EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_PERIOD));
		return getPeriod(periodPointer);
	}
}