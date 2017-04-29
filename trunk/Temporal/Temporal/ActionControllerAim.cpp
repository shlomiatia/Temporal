#include "ActionControllerAim.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "ActionController.h"
#include "Math.h"
#include "Grid.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Aim::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void Aim::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FIRE)
			{
				// Temp flag 1 - Firing
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_HOLSTER)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
			else if (message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				if (!result.getFixture().getCategory() == CollisionCategory::PLAYER)
					return;

				const Vector& sourcePosition = getPosition(_stateMachine->getEntity());
				const Vector& targetPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(result.getFixture().getEntityId(), Message(MessageID::GET_POSITION)));
				Vector vector = targetPosition - sourcePosition;

				Side::Enum sourceSide = getOrientation(_stateMachine->getEntity());
				if (sourceSide == Side::LEFT)
					vector.setX(-vector.getX());

				float direction = vector.getAngle();
				float weight = direction / PI + 0.5f;
				if (_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(ActionControllerStateIds::FIRE_STATE, &weight);
				}
				else
				{
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_DOWN_ANIMATION, false, 0.0f, 1)));
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_UP_ANIMATION, false, 0.0f, 2, weight)));
				}
			}
		}
	}
}
