#include "ActionControllerDie.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "PhysicsEnums.h"
#include "DynamicBody.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Die::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DIE_ANIMATION)));
			int category = CollisionCategory::DEAD | CollisionCategory::DRAGGABLE;
			_stateMachine->raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &category));
			int dynamicObstacleCollisionCategory = CollisionCategory::DYNAMIC_OBSTACLE;
			_stateMachine->raiseMessage(Message(MessageID::REMOVE_FROM_COLLISION_MASK, &dynamicObstacleCollisionCategory));
		}

		void Die::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				float normalizedTime;
				_stateMachine->raiseMessage(Message(MessageID::GET_ANIMATION_NORMALIZED_TIME, &normalizedTime));
				DynamicBody& body = *static_cast<DynamicBody*>(_stateMachine->getEntity().get(DynamicBody::TYPE));
				body.getFixture().getLocalShape().setRadius(Vector(body.getFixture().getLocalShape().getRadiusY()  *normalizedTime, body.getFixture().getLocalShape().getRadiusY()));
				_stateMachine->getEntity().getManager().getGameState().getGrid().update(&body.getFixture());
			}
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::DEAD_STATE);
			}
		}
	}
}
