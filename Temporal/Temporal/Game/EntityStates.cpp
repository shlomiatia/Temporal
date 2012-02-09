#include "EntityStates.h"
#include "JumpAngles.h"
#include <math.h>
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void Stand::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
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
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = false;	
		}
		// TODO: Consider creating new state for descending, or quering controller directly
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
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
	}

	void Walk::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ACTION_UP)
		{
			EntityStateID::Enum defaultJumpStartState = EntityStateID::JUMP_START_45;
			_stateMachine->changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
		}
		else if(message.getID() == MessageID::ENTER_STATE || message.getID() == MessageID::ACTION_FORWARD)
		{
			// TODO: Consider querying controller directly for walk
			_stillWalking = true;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_stillWalking)
			{
				_stateMachine->changeState(EntityStateID::STAND);
			}
			else
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(_stateMachine->WALK_FORCE, 0.0f)));
				_stillWalking = false;
			}
		}
	}

	void Turn::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(EntityStateID::STAND);
		}
	}

	const EntityStateID::Enum PrepareToJump::JUMP_ANGLES_START_STATES[] = { EntityStateID::JUMP_START_45, EntityStateID::JUMP_START_60, EntityStateID::JUMP_START_75, EntityStateID::JUMP_START_105 };

	void PrepareToJump::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);

		if(message.getID() == MessageID::ENTER_STATE)
		{
			_jumpStartState = *(const EntityStateID::Enum* const)message.getParam();
		}
		else if(isSensorMessage(message, SensorID::JUMP))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			bool platformFound = false;
			_stateMachine->changeState(_jumpStartState, &platformFound);
		}
	}

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

	void JumpStart::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = *(const bool* const)message.getParam();
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			float jumpForceX = _stateMachine->JUMP_FORCE * cos(_angle);
			float jumpForceY = _stateMachine->JUMP_FORCE * sin(_angle);
			Vector jumpForce(jumpForceX, jumpForceY);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &jumpForce));
			_stateMachine->changeState(_jumpState);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			// TODO: Constants for each angle SLOTH!
			if(_angle != DEGREES_45 && !_platformFound)
				_stateMachine->changeState(EntityStateID::JUMP_START_45, &_platformFound);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(EntityStateID::TURN);
		}
	}

	void JumpUp::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::TOP))
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			const Vector& force = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_FORCE));
			if(force.getY() <= 0.0f)
				_stateMachine->changeState(EntityStateID::STAND);
		}
	}

	void JumpForward::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(EntityStateID::JUMP_FORWARD_END);
		}
	}

	void JumpForwardEnd::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
	}

	void Hanging::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::HANG);
		}
	}

	// TODO: Maybe need to divide it like prepare to desecend
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

	void Hang::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine->changeState(EntityStateID::DROP);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(EntityStateID::CLIMB);
		}
	}

	void Drop::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isSensorMessage(message, SensorID::HANG))
		{
			// TODO: Query sensor
			_platformFound = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = false;
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
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SIZE));
			float climbForceX = 1.0f;
			float climbForceY = size.getHeight() - 1.0f;
			Vector climbForce(climbForceX, climbForceY);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &climbForce));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->resetDrawPositionOverride();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(EntityStateID::STAND);
		}
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
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

	void Descend::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_SIZE));
			float forceX = -1.0f;
			float forceY = -(size.getHeight() - 1.0f);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
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