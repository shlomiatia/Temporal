#include "Patrol.h"
#include <Temporal\Game\MessageUtils.h>
#include <Temporal\Game\Message.h>

namespace Temporal
{
	namespace PatrolStates
	{
		static const Hash FRONT_EDGE_SENSOR_ID = Hash("SENS_FRONT_EDGE");

		static const Hash WALK_STATE = Hash("STAT_PAT_WALK");
		static const Hash SEE_STATE = Hash("STAT_PAT_SEE");
		static const Hash TURN_STATE = Hash("STAT_PAT_TURN");
		static const Hash WAIT_STATE = Hash("STAT_PAT_WAIT");

		static const Hash ACTION_TURN_STATE = Hash("STAT_ACT_TURN");

		void Walk::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(SEE_STATE);
			}
			else if(isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
			{
				_stateMachine->changeState(WAIT_STATE);
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
					_stateMachine->changeState(WALK_STATE);
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
				const Hash& actionID = *(Hash*)message.getParam();
				if(actionID == ACTION_TURN_STATE)
					_stateMachine->changeState(WALK_STATE);
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
				_stateMachine->changeState(SEE_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);

				if(_timer.getElapsedTimeInMillis() >= WAIT_TIME_IN_MILLIS)
				{
					_stateMachine->changeState(TURN_STATE);
				}
			}
		}
	}
	using namespace PatrolStates;

	StateCollection Patrol::getStates(void) const
	{
		StateCollection states;
		
		states[WALK_STATE] = new Walk();
		states[SEE_STATE] = new See();
		states[TURN_STATE] = new Turn();
		states[WAIT_STATE] = new Wait();
		return states;
	}

	Hash Patrol::getInitialState(void) const
	{
		return WALK_STATE;
	}
}