#include "Patrol.h"
#include "Vector.h"
#include "MessageUtils.h"

namespace Temporal
{
	namespace PatrolStates
	{
		static const Hash FRONT_EDGE_SENSOR_ID = Hash("SNS_FRONT_EDGE");

		static const Hash WALK_STATE = Hash("PAT_STT_WALK");
		static const Hash SEE_STATE = Hash("PAT_STT_SEE");
		static const Hash TURN_STATE = Hash("PAT_STT_TURN");
		static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");

		static const Hash ACTION_TURN_STATE = Hash("ACT_STT_TURN");

		void Walk::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(SEE_STATE);
			}
			else if(isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
			{
				_stateMachine->changeState(WAIT_STATE);
			}
			else if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if(collision.getX() != 0.0f)
				{
					_stateMachine->changeState(WAIT_STATE);
				}
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}

		void See::enter() const
		{
			// TempFlag1 - have line of sight
			_stateMachine->setTempFlag1(true);
		}

		void See::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setTempFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getTempFlag1())
					_stateMachine->changeState(WALK_STATE);
			}
		}

		void Turn::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_BACKWARD));
		}

		void Turn::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::STATE_EXITED)
			{
				Hash actionID = getHashParam(message.getParam());
				if(actionID == ACTION_TURN_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
		}

		const float Wait::WAIT_TIME(5.0f);

		void Wait::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(SEE_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(_stateMachine->getTimer().getElapsedTime() >= WAIT_TIME)
				{
					_stateMachine->changeState(TURN_STATE);
				}
			}
		}
	}
	using namespace PatrolStates;

	StateCollection Patrol::getStates() const
	{
		StateCollection states;
		
		states[WALK_STATE] = new Walk();
		states[SEE_STATE] = new See();
		states[TURN_STATE] = new Turn();
		states[WAIT_STATE] = new Wait();
		return states;
	}

	Hash Patrol::getInitialState() const
	{
		return WALK_STATE;
	}
}