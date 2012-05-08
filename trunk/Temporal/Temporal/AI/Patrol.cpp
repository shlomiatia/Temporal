#include "Patrol.h"
#include <Temporal\Game\MessageParams.h>
#include <Temporal\Game\Message.h>

namespace Temporal
{
	namespace PatrolStates
	{
		static const Hash FRONT_EDGE_SENSOR_ID = Hash("SI_FRONT_EDGE");

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
		void Walk::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(PatrolStates::SEE);
			}
			else if(isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
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
			if(message.getID() == MessageID::STATE_EXITED && message.getSender() == ComponentType::ACTION_CONTROLLER)
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

	StateCollection Patrol::getStates(void) const
	{
		StateCollection states;
		using namespace PatrolStates;
		states.push_back(new Walk());
		states.push_back(new See());
		states.push_back(new Turn());
		states.push_back(new Wait());
		return states;
	}

}