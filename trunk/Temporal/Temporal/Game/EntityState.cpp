#include <Temporal\Base\Base.h>
#include "EntityState.h"
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void EntityState::handleGravity(bool gravityEnabled) const
	{
		if(_gravityResponse == EntityStateGravityResponse::DISABLE_GRAVITY)
		{
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
	}

	bool EntityState::isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative) const
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			Direction::Enum direction = message.getParam<Direction::Enum>();
			if(match(direction, positive, negative))
				return true;
		}
		return false;
	}

	bool EntityState::isSensorMessage(Message& message, SensorID::Enum sensorID) const
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const Sensor& sensor = message.getParam<Sensor>();
			if(sensor.getID() == sensorID)
				return true;
		}
		return NULL;
	}

	void EntityState::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			handleGravity(false);
			_stateMachine.sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &_animation));
			if(_stopForce)
				_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			handleGravity(true);
		}
		else if(_gravityResponse == EntityStateGravityResponse::FALL && isBodyCollisionMessage(message, Direction::ALL, Direction::BOTTOM))
		{
			_stateMachine.changeState(EntityStateID::FALL);
		}
		else if(_supportsHang && isSensorMessage(message, SensorID::HANG))
		{
			_stateMachine.changeState(EntityStateID::HANGING, &message.getParam<Sensor>());
		}
	}
}
