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
				const Sensor& sensor = *(Sensor*)message.getParam();
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

		void See::enter(void)
		{
			_haveLineOfSight = true;
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
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

		void Turn::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
		}

		void Turn::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const ActionStateID::Enum& actionID = *(ActionStateID::Enum*)message.getParam();
				if(actionID == ActionStateID::TURN)
					_stateMachine->changeState(PatrolStates::WALK);
			}
		}

		const float Wait::WAIT_TIME_IN_MILLIS(5000.0f);

		void Wait::enter(void)
		{
			_timer.reset();
		}

		void Wait::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(PatrolStates::SEE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);

				if(_timer.getElapsedTimeInMillis() >= WAIT_TIME_IN_MILLIS)
				{
					_stateMachine->changeState(PatrolStates::TURN);
				}
			}
		}
	}

	std::vector<ComponentState*> Patrol::getStates(void) const
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