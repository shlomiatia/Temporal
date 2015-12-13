#include "SecurityCamera.h"
#include "MessageUtils.h"
#include "Transform.h"
#include "CollisionFilter.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	const Hash SecurityCamera::TYPE = Hash("security-camera");
	const Hash SecurityCamera::ALARM_TARGET_ID = Hash("ENT_ALARM_TARGET_ID");

	namespace SecurityCameraStates
	{
		static Hash SEARCH_ANIMATION = Hash("CAM_ANM_SEARCH");
		static Hash SEE_ANIMATION = Hash("CAM_ANM_SEE");
		static Hash ACQUIRE_ANIMATION = Hash("CAM_ANM_ACQUIRE");

		static const Hash SEARCH_STATE = Hash("CAM_STT_SEARCH");
		static const Hash SEE_STATE = Hash("CAM_STT_SEE");
		static const Hash ACQUIRE_STATE = Hash("CAM_STT_ACQUIRE");

		static const Hash TIME_MACHINE_ID = Hash("ENT_TIME_MACHINE0");

		SecurityCamera& getSecurityCamera(StateMachineComponent* stateMachine)
		{
			return *static_cast<SecurityCamera*>(stateMachine);
		}

		void Search::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(SEARCH_ANIMATION)));
		}

		void Search::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
		}

		bool See::samePeriod(Entity& entity)
		{
			void* result = entity.handleMessage(Message(MessageID::GET_COLLISION_GROUP));
			if (result)
			{
				int targetCollsionGroup = getIntParam(result);
				int sourceCollisionGroup = getIntParam(_stateMachine->raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
				return sourceCollisionGroup == targetCollsionGroup;
			}
		}

		void See::enter(void* param)
		{
			Hash cameraTargetId = SecurityCamera::ALARM_TARGET_ID;
			Entity* timeMachine = _stateMachine->getEntity().getManager().getEntity(TIME_MACHINE_ID);
			_stateMachine->getEntity().getManager().sendMessageToAllEntities(Message(MessageID::ALARM, &cameraTargetId), 0, timeMachine ? 0 : createFunc1(See, bool, Entity&, samePeriod));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(SEE_ANIMATION)));
		}

		void See::handleMessage(Message& message)
		{
			// TempFlag 1 - Have line of sight
			if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				getSecurityCamera(_stateMachine).trackTarget(result);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (!_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(SEARCH_STATE);
				}
			}
		}

		const float Acquire::ACQUIRE_TIME(1.0f);

		void Acquire::enter(void* param)
		{
			// TempFlag 1 - have LOS
			_stateMachine->setFrameFlag1(true);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(ACQUIRE_ANIMATION)));
		}

		void Acquire::update() const
		{
			float elapsedTime = _stateMachine->getTimer().getElapsedTime();
			if (!_stateMachine->getFrameFlag1())
				_stateMachine->changeState(SEARCH_STATE);
			else if (elapsedTime >= ACQUIRE_TIME)
				_stateMachine->changeState(SEE_STATE);
		}

		void Acquire::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				getSecurityCamera(_stateMachine).trackTarget(result);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				update();
			}
		}
	}

	using namespace SecurityCameraStates;

	HashStateMap SecurityCamera::getStates() const
	{
		HashStateMap states;
		states[SEARCH_STATE] = new Search();
		states[SEE_STATE] = new See();
		states[ACQUIRE_STATE] = new Acquire();
		return states;
	}

	Hash SecurityCamera::getInitialState() const
	{
		return SEARCH_STATE;
	}

	void SecurityCamera::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_READY)
		{
			Entity* existing = getEntity().getManager().getEntity(SecurityCamera::ALARM_TARGET_ID);
			if (existing)
				return;
			int categoryId = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_CATEGORY)));
			int groupId = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			Entity* entity = new Entity();
			entity->setBypassSave(true);
			entity->add(new Transform());
			entity->add(new CollisionFilter(categoryId, groupId));
			entity->add(new StaticBody(new Fixture(OBBAABB(Vector::Zero, Vector(5.0f, 5.0f)))));
			entity->setId(SecurityCamera::ALARM_TARGET_ID);
			getEntity().getManager().add(entity);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			Entity* existing = getEntity().getManager().getEntity(SecurityCamera::ALARM_TARGET_ID);
			if (existing)
				getEntity().getManager().remove(SecurityCamera::ALARM_TARGET_ID);
		}
	}

	void SecurityCamera::trackTarget(RayCastResult& result)
	{
		Vector position = result.getDirectedSegment().getTarget();
		getEntity().getManager().sendMessageToEntity(ALARM_TARGET_ID, Message(MessageID::SET_POSITION, &position));
		setFrameFlag1(true);
	}
}