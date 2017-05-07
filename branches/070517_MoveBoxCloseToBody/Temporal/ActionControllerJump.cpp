#include "ActionControllerJump.h"
#include "JumpInfoByTypeProvider.h"
#include "ActionController.h"
#include "MessageUtils.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Jump::enter(void* param)
		{
			const JumpInfoByTypeProvider& jumpHelper = *static_cast<JumpInfoByTypeProvider*>(param);
			float angle = jumpHelper.getInfo().getAngle();
			Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
			float force = ActionController::JUMP_FORCE_PER_SECOND + velocity.getLength();
			float max = ActionController::JUMP_FORCE_PER_SECOND * ActionController::MAX_WALK_JUMP_MODIFIER;
			if (force > max)
				force = max;
			float jumpForceX = force * cos(angle);
			float jumpForceY = force * sin(angle);
			Vector jumpVector = Vector(jumpForceX, jumpForceY);
			_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &jumpVector));
			Hash animation = jumpHelper.getInfo().getJumpAnimation();
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(animation)));

			// Pressed on transition
			_stateMachine->setFrameFlag1(true);
		}

		void Jump::handleMessage(Message& message)
		{
			// TempFlag 1 - Want to hang
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_UP_CONTINUE)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::SENSOR_HANG_UP)
			{
				if (_stateMachine->getFrameFlag1())
					_stateMachine->changeState(ActionControllerStateIds::HANG_STATE, message.getParam());
			}
			else if (message.getID() == MessageID::SENSOR_HANG_FRONT)
			{
				_stateMachine->changeState(ActionControllerStateIds::CLIMB_STATE, message.getParam());
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				// StateFlag 1 - stopped jumping
				if (!_stateMachine->getStateFlag() && !_stateMachine->getFrameFlag1())
				{
					Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
					if (velocity.getY() > 0.0f)
					{
						velocity.setY(velocity.getY() * ActionController::JUMP_STOP_MODIFIER);
						_stateMachine->setStateFlag(true);
					}
				}
			}
		}
	}
}
