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
	static const Hash AIM_STATE = Hash("PAT_STT_AIM");
	static const Hash FIRE_STATE = Hash("PAT_STT_FIRE");
	static const Hash TURN_STATE = Hash("PAT_STT_TURN");
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");
	static const Hash TAKEDOWN_STATE = Hash("PAT_STT_TAKEDOWN");
	static const Hash NAVIGATE_STATE = Hash("PAT_STT_NAVIGATE");
	static const Hash DEAD_STATE = Hash("PAT_STT_DEAD");

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

	Patrol::Patrol(Hash securityCameraId, bool isStatic, Hash initialStateId) :
		StateMachineComponent(getStates(), "PAT", initialStateId), _edgeDetector(FRONT_EDGE_SENSOR_ID, *this), _isStatic(isStatic), _securityCameraId(securityCameraId) {}

	void Patrol::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::DIE)
		{
			changeState(DEAD_STATE);
		}
		_edgeDetector.handleMessage(message);
		StateMachineComponent::handleMessage(message);
	}

	bool Patrol::handleAlarmMessage(Message& message)
	{
		if (message.getID() == MessageID::LINE_OF_SIGHT)
		{
			RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
			if (result.getFixture().getCategory() & CollisionCategory::DEAD)
			{
				Hash id = result.getFixture().getEntityId();
				if (!getBoolParam(getEntity().getManager().sendMessageToEntity(id, Message(MessageID::IS_INVESTIGATED))))
				{
					raiseMessage(Message(MessageID::NAVIGATE, &id));
				}
			}
			return true;
		}
		else if (message.getID() == MessageID::ALARM)
		{
			const SourceTargetParams& params = getSourceTargetParams(message.getParam());
			Hash securityCameraId = params.getSourceId();

			if (getSecurityCameraId() == Hash::INVALID || getSecurityCameraId() == securityCameraId)
			{
				Hash alarmTargetId = params.getTargetId();
				raiseMessage(Message(MessageID::NAVIGATE, &alarmTargetId));
			}
			return true;
		}
		else if (message.getID() == MessageID::NAVIGATION_START)
		{
			changeState(NAVIGATE_STATE);
			return true;
		}
		
		return false;
	}

	bool Patrol::handleFireMessage(Message& message)
	{
		if (message.getID() == MessageID::LINE_OF_SIGHT)
		{
			bool canAttack = false;
			raiseMessage(Message(MessageID::CAN_ATTACK, &canAttack));
			if (!canAttack)
				return false;

			RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
			if (result.getFixture().getCategory() == CollisionCategory::PLAYER)
			{
				changeState(AIM_STATE);
				return true;
			}
			
		}
		return false;
	}

	bool Patrol::handleTakedownMessage(Message& message)
	{
		if (message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == TAKEDOWN_SENSOR_ID)
			{
				bool canAttack = false;
				raiseMessage(Message(MessageID::CAN_ATTACK, &canAttack));
				if (!canAttack)
					return false;
				
				void* isVisible = getEntity().getManager().sendMessageToEntity(params.getContact().getTarget().getEntityId(), Message(MessageID::IS_VISIBLE));
				if (isVisible && !getBoolParam(isVisible))
					return false;

				changeState(TAKEDOWN_STATE);
				return true;
			}
		}
		return false;
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
		states[AIM_STATE] = new Aim();
		states[FIRE_STATE] = new Fire();
		states[TURN_STATE] = new Turn();
		states[WAIT_STATE] = new Wait();
		states[TAKEDOWN_STATE] = new Takedown();
		states[NAVIGATE_STATE] = new Navigate();
		states[DEAD_STATE] = new Dead();
		return states;
	}

	namespace PatrolStates
	{
		void Walk::handleMessage(Message& message)
		{	
			if (getPatrol(_stateMachine).handleTakedownMessage(message) ||
				getPatrol(_stateMachine).handleFireMessage(message) ||
				getPatrol(_stateMachine).handleAlarmMessage(message))
			{
			}
			else if(message.getID() == MessageID::COLLISIONS_CORRECTED)
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

		const float Aim::AIM_TIME(0.5f);

		void Aim::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_AIM));
			// TempFlag 1 - LOS
			_stateMachine->setFrameFlag1(true);
		}

		void Aim::handleMessage(Message& message)
		{
			if (getPatrol(_stateMachine).handleTakedownMessage(message))
			{

			}
			else if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				if (result.getFixture().getCategory() == CollisionCategory::PLAYER)
					_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(_stateMachine->getTimer().getElapsedTime() >= AIM_TIME) 
				{
					if(!_stateMachine->getFrameFlag1())
					{
						_stateMachine->changeState(WAIT_STATE);
					}
					else
					{
						_stateMachine->changeState(FIRE_STATE);
					}
				}
			}
		}

		void Fire::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::STOP_NAVIGATE));
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FIRE));
		}

		void Fire::handleMessage(Message& message)
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

		void Wait::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_HOLSTER));
		}

		void Wait::handleMessage(Message& message)
		{
			if (getPatrol(_stateMachine).handleTakedownMessage(message) ||
				getPatrol(_stateMachine).handleFireMessage(message) ||
				getPatrol(_stateMachine).handleAlarmMessage(message))
			{

			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if (!getPatrol(_stateMachine).isStatic() && _stateMachine->getTimer().getElapsedTime() >= WAIT_TIME)
				{
					_stateMachine->changeState(TURN_STATE);
				}
			}
		}

		void Takedown::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::STOP_NAVIGATE));
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
			getPatrol(_stateMachine).handleFireMessage(message);
			getPatrol(_stateMachine).handleTakedownMessage(message);

			if (message.getID() == MessageID::NAVIGATION_SUCCESS)
			{
				Hash id = getHashParam(message.getParam());
				
				CollisionCategory::Enum category = *static_cast<CollisionCategory::Enum*>(_stateMachine->getEntity().getManager().sendMessageToEntity(id, Message(MessageID::GET_COLLISION_CATEGORY)));
				if (category & CollisionCategory::DEAD)
				{
					_stateMachine->getEntity().getManager().sendMessageToEntity(id, Message(MessageID::INVESTIGATE));
					_stateMachine->raiseMessage(Message(MessageID::ACTION_INVESTIGATE));
				}
				_stateMachine->changeState(WAIT_STATE);
			}
		}
	}
}