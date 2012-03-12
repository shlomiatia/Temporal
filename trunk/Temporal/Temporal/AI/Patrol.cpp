#include "Patrol.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Physics\Sensor.h>

namespace Temporal
{
	namespace PatrolStates
	{
		bool isSensorMessage(Message& message, SensorID::Enum sensorID)
		{
			if(message.getID() == MessageID::SENSOR_COLLISION)
			{
				const Sensor& sensor = *(const Sensor* const)message.getParam();
				if(sensor.getID() == sensorID)
					return true;
			}
			return NULL;
		}

		void Walk::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(PatrolStates::SEE);
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE))
			{
				_stateMachine->changeState(PatrolStates::WAIT);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
			}
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_haveLineOfSight)
					_stateMachine->changeState(PatrolStates::WALK);
				_haveLineOfSight = false;
			}
		}

		void Turn::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
			}
			else if(message.getID() == MessageID::FLIP_ORIENTATION)
			{
				_stateMachine->changeState(PatrolStates::WALK);
			}
		}

		const float Wait::WAIT_TIME_IN_MILLIS(5000.0f);

		void Wait::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_timer.reset();
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(PatrolStates::SEE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(const float* const)message.getParam();
				_timer.update(framePeriodInMillis);

				if(_timer.getElapsedTimeInMillis() >= WAIT_TIME_IN_MILLIS)
				{
					_stateMachine->changeState(PatrolStates::TURN);
				}
			}
		}
	}

	std::vector<ComponentState*> Patrol::getStates() const
	{
		std::vector<ComponentState*> states;
		using namespace PatrolStates;
		states.push_back(new Walk());
		states.push_back(new See());
		states.push_back(new Turn());
		states.push_back(new Wait());
		return states;
	}

}