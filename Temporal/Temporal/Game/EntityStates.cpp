#include "EntityStates.h"
#include "JumpAngles.h"
#include <math.h>
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void Stand::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine->changeState(EntityStateID::WALK);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(EntityStateID::TURN);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{
			EntityStateID::Enum defaultJumpStartState = EntityStateID::JUMP_START_90;
			_stateMachine->changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
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
				_stateMachine->changeState(EntityStateID::PREPARE_TO_DESCEND, &sensor);
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE) != NULL)
			{
				_stateMachine->changeState(EntityStateID::TURN);
			}
		}
	}

	void Fall::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(EntityStateID::HANGING, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::FALL)));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
	}

	void Walk::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			EntityStateID::Enum defaultJumpStartState = EntityStateID::JUMP_START_45;
			_stateMachine->changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stillWalking = true;
		}
		else if(isBodyCollisionMessage(message, Direction::ALL, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::FALL);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Consider querying controller directly for walk
			_stillWalking = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::WALK, false, true)));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			EntityStateID::Enum newState = *(const EntityStateID::Enum* const)message.getParam();
			if(newState != EntityStateID::FALL)
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_stillWalking)
			{
				_stateMachine->changeState(EntityStateID::STAND);
			}
			else
			{
				// TODO: Move to enter state when there will be walk start state
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(_stateMachine->WALK_FORCE, 0.0f)));
				_stillWalking = false;
			}
		}
	}

	void Turn::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::TURN)));
		}
	}

	const EntityStateID::Enum PrepareToJump::JUMP_ANGLES_START_STATES[] = { EntityStateID::JUMP_START_45, EntityStateID::JUMP_START_60, EntityStateID::JUMP_START_75, EntityStateID::JUMP_START_105 };

	void PrepareToJump::handleJumpSensor(Message &message)
	{
		// TODO: Transfer sensor message param with entity id, and query everything
		const Sensor& sensor = *(const Sensor* const)message.getParam();
		const Body& sensorOwner = sensor.getOwner();
		const Body* const sensedBody = sensor.getSensedBody();
		SensorID::Enum hangSensorID = SensorID::HANG;
		const Vector& hangSensorSize = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SENSOR_SIZE, &hangSensorID));
		float hangSensorWidth = hangSensorSize.getWidth();
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		float target = sensedBody->getBounds().getOppositeSide(orientation);
		float front = sensorOwner.getBounds().getSide(orientation);
		float x = (target - front) * orientation;
		bool platformFound = false;

		if(x >= 0 && x < hangSensorWidth - 1.0f)
		{
			_jumpStartState = EntityStateID::JUMP_START_90;
			platformFound = true;
		}
		else
		{
			const float F = _stateMachine->JUMP_FORCE;
			float max = 0.0f;
			float G = *(const float* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_GRAVITY));
			for(int i = 0; i < JUMP_ANGLES_SIZE; ++i)
			{
				/* x = Time*Force*cos(Angle)
				* y = Time*(-Gravity*Time+Force*sin(Angle))
				* 
				* T = x/(F*cos(A)) // Isolate T
				* y = (-(G*x/F*cos(A)+x/F*cos(A))*(F*sin(A)) // Place T 
				* y = (-G*x^2/F*cos(A) + F*sin(A)*x)/F*cos(A) // Multiply
				* y = (-G*x^2 + F^2*sin(A)*cos(A)*x)/(F*cos(A))/F*cos(A) // Multiply with common denominator
				* y = (-G*x^2 + F^2*sin(A)*cos(A)*x)/(F*cos(A))^2
				*/
				float A = JUMP_ANGLES[i];
				float y = (-G*pow(x, 2) + pow(F, 2)*sin(A)*cos(A)*x)/pow(F*cos(A), 2);
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
			_jumpStartState = *(const EntityStateID::Enum* const)message.getParam();
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
			float jumpForceX = _stateMachine->JUMP_FORCE * cos(_angle);
			float jumpForceY = _stateMachine->JUMP_FORCE * sin(_angle);
			Vector jumpForce(jumpForceX, jumpForceY);

			// TODO: Move to jump states for clarity
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &jumpForce));
			_stateMachine->changeState(_jumpState);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			if(_angle != DEGREES_45 && !_platformFound)
				_stateMachine->changeState(EntityStateID::JUMP_START_45, &_platformFound);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(EntityStateID::TURN);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = *(const bool* const)message.getParam();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(_animation)));
		}
	}

	void JumpUp::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(EntityStateID::HANGING, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_UP)));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
	}

	void JumpForward::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(EntityStateID::HANGING, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::JUMP_FORWARD_END);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_FORWARD)));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
	}

	void JumpForwardEnd::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::JUMP_FORWARD_END)));
		}
	}

	void Hanging::update(void)
	{
		const Rect& personBounds = _person->getBounds();
		const Rect& platformBounds = _platform->getBounds();
		float platformTop = platformBounds.getTop();
		float entityTop = personBounds.getTop();
		float moveY = platformTop - entityTop;

		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		float platformEdge = platformBounds.getOppositeSide(orientation);
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		Vector move(moveX, moveY);
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &move));

		float personCenterX = personBounds.getCenterX();
		Vector drawPosition(personCenterX, platformTop);
		_stateMachine->setDrawPositionOverride(drawPosition);
	}

	void Hanging::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::HANG);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();

			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::HANG_FORWARD, true)));
			bool gravityEnabled = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		
	}

	void Hang::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine->changeState(EntityStateID::DROP);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(EntityStateID::CLIMB);
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
				_stateMachine->changeState(EntityStateID::STAND);
			else
				_platformFound = false;
		}
	}

	void Climb::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SIZE));
			float climbForceX = 1.0f;
			float climbForceY = size.getHeight() - 1.0f;
			Vector climbForce(climbForceX, climbForceY);

			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB)));
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &climbForce));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->resetDrawPositionOverride();
			bool gravityEnabled = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
	}

	void PrepareToDescend::update(void)
	{
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Rect& personBounds = _person->getBounds();
		const Rect& platformBounds = _platform->getBounds();
		float platformEdge = platformBounds.getOppositeSide(orientation) + 1.0f * orientation;
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(moveX, 0.0f)));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = platformBounds.getTop();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->setDrawPositionOverride(drawPosition);

			_stateMachine->changeState(EntityStateID::DESCEND);
		}
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_person = &sensor.getOwner();
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

			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB, true)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::HANG);
		}
	}

}