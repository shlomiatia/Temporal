#include "EntityStates.h"
#include <math.h>
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	void Stand::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine.changeState(EntityStateID::WALK);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine.changeState(EntityStateID::TURN);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{
			EntityStateID::Enum defaultJumpStartState = EntityStateID::JUMP_START_90;
			_stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
		}
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine.resetDrawPositionOverride();
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
				_stateMachine.changeState(EntityStateID::PREPARE_TO_DESCEND, &message.getParam<Sensor>());
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE) != NULL)
			{
				_stateMachine.changeState(EntityStateID::TURN);
			}
		}
	}

	void Fall::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void Walk::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ACTION_UP)
		{
			EntityStateID::Enum defaultJumpStartState = EntityStateID::JUMP_START_45;
			_stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
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
				_stateMachine.changeState(EntityStateID::STAND);
			}
			else
			{
				_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(_stateMachine.WALK_FORCE, 0.0f)));
				_stillWalking = false;
			}
		}
	}

	void Turn::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine.sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine.changeState(EntityStateID::STAND);
		}
	}

	// TODO: Convert angles to std:vector SLOTH!
	const float PrepareToJump::ANGLES_SIZE = 4;
	const float PrepareToJump::ANGLES[] = { toRadians(45.0f), toRadians(60.0f), toRadians(75.0f), toRadians(105.0) };
	const EntityStateID::Enum PrepareToJump::JUMP_START_STATES[] = { EntityStateID::JUMP_START_45, EntityStateID::JUMP_START_60, EntityStateID::JUMP_START_75, EntityStateID::JUMP_START_105 };

	void PrepareToJump::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);

		if(message.getID() == MessageID::ENTER_STATE)
		{
			_jumpStartState = message.getParam<EntityStateID::Enum>();
		}
		else if(isSensorMessage(message, SensorID::JUMP))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			bool platformFound = false;
			_stateMachine.changeState(_jumpStartState, &platformFound);
		}
	}

	void PrepareToJump::handleJumpSensor(Message &message)
	{
		// TODO: Transfer sensor message param with entity id, and query everything
		const Sensor& sensor = message.getParam<Sensor>();
		const Body& sensorOwner = sensor.getOwner();
		const Body* const sensedBody = sensor.getSensedBody();
		SensorID::Enum hangSensorID = SensorID::HANG;
		const Vector& hangSensorSize = _stateMachine.sendQueryMessageToOwner<Vector>(Message(MessageID::GET_SENSOR_SIZE, &hangSensorID));
		float hangSensorWidth = hangSensorSize.getWidth();
		Orientation::Enum orientation = _stateMachine.sendQueryMessageToOwner<Orientation::Enum>(Message(MessageID::GET_ORIENTATION));
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
			const float F = _stateMachine.JUMP_FORCE;
			float max = 0.0f;
			float G = _stateMachine.sendQueryMessageToOwner<float>(Message(MessageID::GET_GRAVITY));
			for(int i = 0; i < ANGLES_SIZE; ++i)
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
				float A = ANGLES[i];
				float y = (-G*pow(x, 2) + pow(F, 2)*sin(A)*cos(A)*x)/pow(F*cos(A), 2);
				if(max < y)
				{
					max = y;
					_jumpStartState = JUMP_START_STATES[i];
					platformFound = true;
				}
			}
		}

		_stateMachine.changeState(_jumpStartState, &platformFound);
	}

	void JumpStart::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = message.getParam<bool>();
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(_stateMachine.JUMP_FORCE*cos(_angle), _stateMachine.JUMP_FORCE*sin(_angle))));
			_stateMachine.changeState(_jumpState);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			// TODO: Constants for each angle SLOTH!
			if(_angle != toRadians(45) && !_platformFound)
				_stateMachine.changeState(EntityStateID::JUMP_START_45, &_platformFound);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine.changeState(EntityStateID::TURN);
		}
	}

	void JumpUp::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::TOP))
		{
			_stateMachine.changeState(EntityStateID::STAND);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			const Vector& force = _stateMachine.sendQueryMessageToOwner<Vector>(Message(MessageID::GET_FORCE));
			if(force.getY() <= 0.0f)
				_stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void JumpForward::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine.changeState(EntityStateID::JUMP_FORWARD_END);
		}
	}

	void JumpForwardEnd::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void Hanging::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = message.getParam<Sensor>();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();

			const Rect& platformBounds = _platform->getBounds();
			float platformTop = platformBounds.getTop();
			const Rect& personBounds = _person->getBounds();
			float personCenterX = personBounds.getCenterX();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine.setDrawPositionOverride(drawPosition);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine.changeState(EntityStateID::HANG);
		}
	}

	void Hanging::update(void)
	{
		const Rect& personBounds = _person->getBounds();
		const Rect& platformBounds = _platform->getBounds();
		float platformTop = platformBounds.getTop();
		float entityTop = personBounds.getTop();
		float moveY = platformTop - entityTop;

		Orientation::Enum orientation = _stateMachine.sendQueryMessageToOwner<Orientation::Enum>(Message(MessageID::GET_ORIENTATION));
		float platformEdge = platformBounds.getOppositeSide(orientation);
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(moveX, moveY)));
	}

	void Hang::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine.changeState(EntityStateID::DROP);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine.changeState(EntityStateID::CLIMBE);
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
			_stateMachine.resetDrawPositionOverride();

			_platformFound = false;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_platformFound)
				_stateMachine.changeState(EntityStateID::STAND);
			else
				_platformFound = false;
		}
	}

	void Climbe::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = _stateMachine.sendQueryMessageToOwner<Vector>(Message(MessageID::GET_SIZE));
			float forceX = 1.0f;
			float forceY = size.getHeight() - 1.0f;
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
			
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			// TODO: Sensor params
			const Sensor& sensor = message.getParam<Sensor>();
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
		Orientation::Enum orientation = _stateMachine.sendQueryMessageToOwner<Orientation::Enum>(Message(MessageID::GET_ORIENTATION));
		const Rect& personBounds = _person->getBounds();
		const Rect& platformBounds = _platform->getBounds();
		float platformEdge = platformBounds.getOppositeSide(orientation) + 1.0f * orientation;
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
		{
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(moveX, 0.0f)));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = platformBounds.getTop();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine.setDrawPositionOverride(drawPosition);

			_stateMachine.changeState(EntityStateID::DESCEND);
		}
	}

	void Descend::handleMessage(Message& message)
	{
		EntityState::handleMessage(message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = _stateMachine.sendQueryMessageToOwner<Vector>(Message(MessageID::GET_SIZE));
			float forceX = -1.0f;
			float forceY = -(size.getHeight() - 1.0f);
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine.sendMessageToOwner(Message(MessageID::SET_FORCE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine.changeState(EntityStateID::HANG);
		}
	}

}