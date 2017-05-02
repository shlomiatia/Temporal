#include "ActionControllerWalk.h"
#include "ActionController.h"
#include "JumpInfoByTypeProvider.h"
#include "Ids.h"
#include "MessageUtils.h"
#include "Math.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Walk::enter(void* param)
		{
			// TempFlag 1 - still walking
			_stateMachine->setFrameFlag1(true);

			// State flag - started animation
			_stateMachine->setStateFlag(false);

			// BRODER
			Vector force = Vector(32.0f, 0.0f);
			_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
		}

		void Walk::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_UP_START)
			{
				JumpInfoByTypeProvider helper(JumpType::FORWARD);
				_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
			}
			// TempFlag 1 - still walking
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_INVESTIGATE)
			{
				_stateMachine->changeState(ActionControllerStateIds::INVESTIGATE_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (!_stateMachine->getStateFlag())
				{
					_stateMachine->setStateFlag(true);
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION)));
				}
				if (!_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
				}
				else
				{
					// We need to apply this every update because the ground has infinite restitution. 
					float x = ActionController::WALK_ACC_PER_SECOND * _stateMachine->getTimer().getElapsedTime();
					if (x > ActionController::getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND)
						x = ActionController::getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND;
					Vector force = Vector(x, 0.0f);
					_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
				}
			}
			else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if (sameSign(side, collision.getX()) && !AngleUtils::radian().isSteep(collision.getAngle()))
				{
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
				}
			}
		}
	}
}
