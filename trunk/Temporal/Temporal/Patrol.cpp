#include "Patrol.h"
#include "Grid.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "PatrolWalk.h"
#include "PatrolAim.h"
#include "PatrolFire.h"
#include "PatrolTurn.h"
#include "PatrolTakedown.h"
#include "PatrolWait.h"
#include "PatrolNavigate.h"
#include "PatrolDead.h"

namespace Temporal
{
	static const Hash FRONT_EDGE_SENSOR_ID = Hash("SNS_FRONT_EDGE");

	static const Hash WALK_STATE = Hash("PAT_STT_WALK");
	static const Hash AIM_STATE = Hash("PAT_STT_AIM");
	static const Hash FIRE_STATE = Hash("PAT_STT_FIRE");
	static const Hash TURN_STATE = Hash("PAT_STT_TURN");
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");
	static const Hash TAKEDOWN_STATE = Hash("PAT_STT_TAKEDOWN");
	static const Hash NAVIGATE_STATE = Hash("PAT_STT_NAVIGATE");
	static const Hash DEAD_STATE = Hash("PAT_STT_DEAD");

	static const Hash TAKEDOWN_SENSOR_ID = Hash("SNS_TAKEDOWN");

	static const Hash PLAYER = Hash("ENT_PLAYER");

	Patrol::Patrol(Hash securityCameraId, bool isStatic, float waitTime, float aimTime, Hash initialStateId) :
		StateMachineComponent(getStates(), "PAT", initialStateId), _edgeDetector(FRONT_EDGE_SENSOR_ID, *this), _isStatic(isStatic), _waitTime(waitTime), _aimTime(aimTime), _securityCameraId(securityCameraId), _isChaser(true) {}

	void Patrol::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::DIE)
		{
			changeState(DEAD_STATE);
		}
		_edgeDetector.handleMessage(message);
		if (message.getID() == MessageID::UPDATE)
		{
			if (_isChaser && getCurrentStateID() != NAVIGATE_STATE)
			{
				Hash targetId = PLAYER;
				raiseMessage(Message(MessageID::NAVIGATE, &targetId));
			}
		}
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

			if (_securityCameraId == Hash::INVALID || _securityCameraId == securityCameraId)
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
		if (message.getID() == MessageID::LINE_OF_SIGHT && !_isChaser)
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
}
