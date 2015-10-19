#include "Patrol.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	const Hash Patrol::TYPE = Hash("patrol");

	static const Hash FRONT_EDGE_SENSOR_ID = Hash("SNS_FRONT_EDGE");

	static const Hash WALK_STATE = Hash("PAT_STT_WALK");
	static const Hash ACQUIRE_STATE = Hash("PAT_STT_ACQUIRE");
	static const Hash SEE_STATE = Hash("PAT_STT_SEE");
	static const Hash TURN_STATE = Hash("PAT_STT_TURN");
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");
	static const Hash TAKEDOWN_STATE = Hash("PAT_STT_TAKEDOWN");
	static const Hash NAVIGATE_STATE = Hash("PAT_STT_NAVIGATE");

	static const Hash ACTION_TURN_STATE = Hash("ACT_STT_TURN");
	static const Hash TAKEDOWN_SENSOR_ID = Hash("SNS_TAKEDOWN");
	static const Hash PATROL_CONTROL_SENSOR_ID = Hash("SNS_PATROL_CONTROL");

	void EdgeDetector::start()
	{
		_isFound = true;
	}

	void EdgeDetector::handle(const Contact& contact)
	{
		_isFound = false;
	}

	void EdgeDetector::end()
	{
		if(_isFound)
			getOwner().raiseMessage(Message(MessageID::SENSOR_FRONG_EDGE));
	}

	Patrol& getPatrol(StateMachineComponent* stateMachine)
	{
		return *static_cast<Patrol*>(stateMachine);
	}

	Patrol::Patrol(bool isStatic) : StateMachineComponent(getStates(), "PAT"), _edgeDetector(FRONT_EDGE_SENSOR_ID, *this), _isStatic(isStatic) {}

	void Patrol::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == TAKEDOWN_SENSOR_ID)
			{
				void* isVisible = getEntity().getManager().sendMessageToEntity(params.getContact().getTarget().getEntityId(), Message(MessageID::IS_VISIBLE));
				if (!isVisible || getBoolParam(isVisible))
				{
					changeState(TAKEDOWN_STATE);
				}
			}
		}
		_edgeDetector.handleMessage(message);
		StateMachineComponent::handleMessage(message);
	}

	void Patrol::handleWaitWalkMessage(Message& message)
	{
		if (message.getID() == MessageID::LINE_OF_SIGHT)
		{
			RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
			if (result.getFixture().getCategory() == CollisionCategory::PLAYER)
			{
				void* ground = raiseMessage(Message(MessageID::GET_GROUND));
				if (!ground)
					return;
				changeState(ACQUIRE_STATE);
			}
			else
			{
				Hash id = result.getFixture().getEntityId();
				if (id != getEntity().getId())
				{
					changeState(NAVIGATE_STATE);
					raiseMessage(Message(MessageID::SET_NAVIGATION_DESTINATION, &id));
				}
				
			}
		}
	}

	Hash Patrol::getInitialState() const
	{
		return WAIT_STATE;
	}

	HashStateMap Patrol::getStates() const
	{
		using namespace PatrolStates;
		HashStateMap states;
		
		states[WALK_STATE] = new Walk();
		states[ACQUIRE_STATE] = new Acquire();
		states[SEE_STATE] = new See();
		states[TURN_STATE] = new Turn();
		states[WAIT_STATE] = new Wait();
		states[TAKEDOWN_STATE] = new Takedown();
		states[NAVIGATE_STATE] = new Navigate();
		return states;
	}

	namespace PatrolStates
	{
		void Walk::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_HOLSTER));
		}

		void Walk::handleMessage(Message& message)
		{	
			getPatrol(_stateMachine).handleWaitWalkMessage(message);
			if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if(collision.getX() != 0.0f && collision.getY() >= 0.0f)
				{
					_stateMachine->changeState(WAIT_STATE);
				}
			}
			else if(message.getID() == MessageID::SENSOR_FRONG_EDGE)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
			else if (message.getID() == MessageID::SENSOR_SENSE)
			{
				const SensorParams& params = getSensorParams(message.getParam());
				if (params.getSensorId() == PATROL_CONTROL_SENSOR_ID)
				{
					_stateMachine->changeState(WAIT_STATE);
				}
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}

		const float Acquire::ACQUIRE_TIME(0.5f);

		void Acquire::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_AIM));
			// TempFlag 1 - LOS
			_stateMachine->setFrameFlag1(true);
		}

		void Acquire::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				if (result.getFixture().getCategory() == CollisionCategory::PLAYER)
					_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(_stateMachine->getTimer().getElapsedTime() >= ACQUIRE_TIME) 
				{
					if(!_stateMachine->getFrameFlag1())
					{
						_stateMachine->changeState(WALK_STATE);
					}
					else
					{
						_stateMachine->changeState(SEE_STATE);
					}
				}
			}
		}

		void See::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FIRE));
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
		}

		void Turn::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_BACKWARD));
		}

		void Turn::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::STATE_EXITED)
			{
				Hash actionID = getHashParam(message.getParam());
				if(actionID == ACTION_TURN_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
		}

		const float Wait::WAIT_TIME(1.0f);

		void Wait::handleMessage(Message& message)
		{
			getPatrol(_stateMachine).handleWaitWalkMessage(message);
			if(message.getID() == MessageID::UPDATE)
			{
				if (!getPatrol(_stateMachine).isStatic() && _stateMachine->getTimer().getElapsedTime() >= WAIT_TIME)
				{
					_stateMachine->changeState(TURN_STATE);
				}
			}
		}

		void Takedown::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_TAKEDOWN));
		}

		void Takedown::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_TAKEDOWN));
			}
			else if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
		}

		void Navigate::handleMessage(Message& message)
		{
			getPatrol(_stateMachine).handleWaitWalkMessage(message);
			if (message.getID() == MessageID::NAVIGATION_DESTINATION_REACHED)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_INVESTIGATE));
				_stateMachine->changeState(WAIT_STATE);
			}
			else if (message.getID() == MessageID::NAVIGATION_DESTINATION_LOST)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
		}
	}
}