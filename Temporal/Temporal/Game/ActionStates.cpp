#include "ActionStates.h"
#include "JumpAngles.h"
#include <math.h>
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void Stand::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine->changeState(ActionStateID::WALK);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(ActionStateID::TURN);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{
			ActionStateID::Enum defaultJumpStartState = ActionStateID::JUMP_START_90;
			_stateMachine->changeState(ActionStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
		}
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_isDescending = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::STAND)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = false;	
		}
		// TODO: Consider creating new state for descending, or querying controller directly
		if(_isDescending)
		{
			if(isSensorMessage(message, SensorID::BACK_EDGE) != NULL)
			{
				const Sensor& sensor = *(const Sensor* const)message.getParam();
				_stateMachine->changeState(ActionStateID::PREPARE_TO_DESCEND, &sensor);
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE) != NULL)
			{
				_stateMachine->changeState(ActionStateID::TURN);
			}
		}
	}

	void Fall::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::FALL)));
		}
	}

	void Walk::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			ActionStateID::Enum defaultJumpStartState = ActionStateID::JUMP_START_45;
			_stateMachine->changeState(ActionStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stillWalking = true;
		}
		else if(isBodyCollisionMessage(message, Direction::ALL, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::FALL);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Consider querying controller directly for walk
			_stillWalking = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::WALK, false, true)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_stillWalking)
			{
				_stateMachine->changeState(ActionStateID::STAND);
			}
			else
			{
				// TODO: Move to enter state when there will be walk start state
				Vector force(_stateMachine->WALK_FORCE_PER_SECOND, 0.0f);
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &force));
				_stillWalking = false;
			}
		}
	}

	void Turn::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::TURN)));
		}
	}

	const ActionStateID::Enum PrepareToJump::JUMP_ANGLES_START_STATES[] = { ActionStateID::JUMP_START_45, ActionStateID::JUMP_START_60, ActionStateID::JUMP_START_75, ActionStateID::JUMP_START_105 };

	void PrepareToJump::handleJumpSensor(Message &message)
	{
		// TODO: Transfer sensor message param with entity id, and query everything
		const Sensor& sensor = *(const Sensor* const)message.getParam();
		const Body* const sensedBody = sensor.getSensedBody();
		SensorID::Enum hangSensorID = SensorID::HANG;
		const Vector& hangSensorSize = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SENSOR_SIZE, &hangSensorID));
		float hangSensorWidth = hangSensorSize.getWidth();
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		float target = sensedBody->getBounds().getOppositeSide(orientation);
		float front = personBounds.getSide(orientation);
		float x = (target - front) * orientation;
		bool platformFound = false;

		if(x >= 0 && x < hangSensorWidth - 1.0f)
		{
			_jumpStartState = ActionStateID::JUMP_START_90;
			platformFound = true;
		}
		else
		{
			float max = 0.0f;
			const float F = _stateMachine->JUMP_FORCE_PER_SECOND;
			const float G = *(const float* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_GRAVITY));
			for(int i = 0; i < JUMP_ANGLES_SIZE; ++i)
			{
				/* x = T*F*cos(A)
				 * y = T*F*sin(A) - (G*T^2)/2
				 *
				 * T = x/(F*cos(A))
				 * y = (x/(F*cos(A))*F*sin(A) - (G*(x/(F*cos(A)))^2)/2
				 * y = (x*sin(A))/(cos(A)) - (G*x^2)/(2*F^2*cos(A)^2)
				 * y = (2*F^2*x*sin(A)*cos(A) - G*x^2)/(2*F^2*cos(A)^2)
				 * y = x*(2*F^2*sin(A)*cos(A) - G*x)/(2*F^2*cos(A)^2)
				 */
				float A = JUMP_ANGLES[i];
				float y = x*(2.0f*pow(F,2.0f)*sin(A)*cos(A) - G*x)/(2.0f*pow(F,2.0f)*pow(cos(A),2.0f));
				if(max < y)
				{
					max = y;
					_jumpStartState = JUMP_ANGLES_START_STATES[i];
					platformFound = true;
				}
			}
		}

		_stateMachine->changeState(_jumpStartState, &platformFound);
	}

	void PrepareToJump::handleMessage(Message& message)
	{
		if(isSensorMessage(message, SensorID::JUMP))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_jumpStartState = *(const ActionStateID::Enum* const)message.getParam();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			bool platformFound = false;
			_stateMachine->changeState(_jumpStartState, &platformFound);
		}
	}

	void JumpStart::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			_animationEnded = true;
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			if(_angle != DEGREES_45 && !_platformFound)
				_stateMachine->changeState(ActionStateID::JUMP_START_45, &_platformFound);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(ActionStateID::TURN);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_animationEnded = false;
			_platformFound = *(const bool* const)message.getParam();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(_animation)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(_animationEnded)
			{
				float jumpForceX = _stateMachine->JUMP_FORCE_PER_SECOND * cos(_angle);
				float jumpForceY = _stateMachine->JUMP_FORCE_PER_SECOND * sin(_angle);
				Vector jumpVector(jumpForceX, jumpForceY);

				// TODO: Move to jump states for clarity
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &jumpVector));
				_stateMachine->changeState(_jumpState);
			}
		}
	}

	void JumpUp::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_UP)));
		}
	}

	void JumpForward::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::JUMP_FORWARD_END);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_FORWARD)));
		}
	}

	void JumpForwardEnd::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_FORWARD_END)));
		}
	}

	void PrepareToHang::update(void)
	{
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Rect& platformBounds = _platform->getBounds();
		float platformTop = platformBounds.getTop();
		float entityTop = personBounds.getTop();
		float movementY = platformTop - entityTop;

		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		float platformEdge = platformBounds.getOppositeSide(orientation);
		float entityFront = personBounds.getSide(orientation);
		float movementX = (platformEdge - entityFront) * orientation;
		Vector movement(movementX, movementY);
		if(movement != Vector::Zero)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &movement));
		}
		else
		{
			float platformTop = platformBounds.getTop();
			float personCenterX = personBounds.getCenterX();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->setDrawPositionOverride(drawPosition);
			_stateMachine->changeState(ActionStateID::HANGING);
		}
	}

	void PrepareToHang::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_platform = sensor.getSensedBody();

			bool gravityEnabled = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Hanging::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::HANG);
		}
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::HANG_BACKWARD, true)));	
		}
	}

	void Hang::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine->changeState(ActionStateID::DROP);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(ActionStateID::CLIMB);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::HANG)));
		}
	}

	void Drop::handleMessage(Message& message)
	{
		if(isSensorMessage(message, SensorID::HANG))
		{
			// TODO: Query sensor
			_platformFound = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::DROP)));

			bool gravityEnabled = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->resetDrawPositionOverride();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_platformFound)
				_stateMachine->changeState(ActionStateID::FALL);
			else
				_platformFound = false;
		}
	}

	void Climb::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SIZE));
			float climbForceX = 1.0f;
			float climbForceY = size.getHeight() - 1.0f;
			Vector climbForce(climbForceX, climbForceY);

			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB)));
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &climbForce));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->resetDrawPositionOverride();
			bool gravityEnabled = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
	}

	void PrepareToDescend::update(void)
	{
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Rect& platformBounds = _platform->getBounds();
		float platformEdge = platformBounds.getOppositeSide(orientation) + 1.0f * orientation;
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &Vector(moveX, 0.0f)));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = platformBounds.getTop();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->setDrawPositionOverride(drawPosition);
			_stateMachine->changeState(ActionStateID::DESCEND);
		}
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_platform = sensor.getSensedBody();

			bool gravityEnabled = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Descend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SIZE));
			float forceX = -1.0f;
			float forceY = -(size.getHeight() - 1.0f);

			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &Vector(forceX, forceY)));
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB, true)));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::HANG);
		}
	}

}