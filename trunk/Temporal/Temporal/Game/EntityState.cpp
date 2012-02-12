#include <Temporal\Base\Base.h>
#include "EntityState.h"
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	bool EntityState::isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative) const
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			Direction::Enum direction = *(const Direction::Enum* const)message.getParam();
			if(match(direction, positive, negative))
				return true;
		}
		return false;
	}

	bool EntityState::isSensorMessage(Message& message, SensorID::Enum sensorID) const
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			if(sensor.getID() == sensorID)
				return true;
		}
		return NULL;
	}
}
