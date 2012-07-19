#include "MessageUtils.h"
#include "EntitySystem.h"
#include "MessageParams.h"
#include "Hash.h"

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
			const SensorCollisionParams& params = *static_cast<SensorCollisionParams*>(message.getParam());
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
		int* periodPointer = static_cast<int*>(component.sendMessageToOwner(Message(MessageID::GET_PERIOD)));
		return getPeriod(periodPointer);
	}

	int getPlayerPeriod()
	{
		int* periodPointer = static_cast<int*>(EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_PERIOD)));
		return getPeriod(periodPointer);
	}
}