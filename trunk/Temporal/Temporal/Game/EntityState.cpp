#include <Temporal\Base\Base.h>
#include "EntityState.h"
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void EntityState::handleGravity(EntityStateMachine& stateMachine, bool gravityEnabled)
	{
		if(_gravityResponse == EntityStateGravityResponse::DISABLE_GRAVITY)
		{
			stateMachine.sendMessage(Message(MessageID::SET_GRAVITY, &gravityEnabled));
		}
	}

	bool isFallingMessage(const Message& message)
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			Direction::Type direction = message.getParam<Direction::Type>();
			if(!(direction & Direction::BOTTOM))
				return true;
		}
		return false;
	}

	bool EntityState::isSensorMessage(Message& message, SensorID::Type sensorID)
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const Sensor& sensor = message.getParam<Sensor>();
			if(sensor.getID() == sensorID)
				return true;
		}
		return NULL;
	}

	void EntityState::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			handleGravity(stateMachine, false);
			stateMachine.sendMessage(Message(MessageID::RESET_ANIMATION, &_animation));
			if(_stopForce)
				stateMachine.sendMessage(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			handleGravity(stateMachine, true);
		}
		else if(_gravityResponse == EntityStateGravityResponse::FALL && isFallingMessage(message))
		{
			stateMachine.changeState(EntityStateID::FALL);
		}
		else if(_supportsHang && isSensorMessage(message, SensorID::HANG))
		{
			stateMachine.changeState(EntityStateID::HANGING);
		}
	}

	void EntityState::sendMessage(EntityStateMachine& stateMachine, Message& message)
	{
		stateMachine.sendMessage(message);
	}
}
