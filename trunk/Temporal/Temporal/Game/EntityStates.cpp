#include "EntityStates.h"
#include <math.h>
#include <Temporal/Physics/Sensor.h>

namespace Temporal
{
	const float JUMP_FORCE = 15.0f;

	bool handleBodyCollisionMessage(Message& message, Direction::Type positive)
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			Direction::Type direction = message.getParam<Direction::Type>();
			if(positive & direction)
				return true;
		}
		return false;
	}

	void Stand::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ACTION_FORWARD):
			{
				stateMachine.changeState(EntityStateID::WALK);
				break;
			}
			case(MessageID::ACTION_BACKWARD):
			{
				stateMachine.changeState(EntityStateID::TURN);
				break;
			}
			case(MessageID::ACTION_UP):
			{
				EntityStateID::Type defaultJumpStartState = EntityStateID::JUMP_START_90;
				stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
				break;
			}
			case(MessageID::ACTION_DOWN):
			{
				_isDescending = true;
				break;
			}
			case (MessageID::ENTER_STATE):
			case(MessageID::UPDATE):
			{
				_isDescending = false;
				break;
			}
		}
		if(_isDescending)
		{
			if(isSensorMessage(message, SensorID::BACK_EDGE) != NULL)
			{
				stateMachine.changeState(EntityStateID::PREPARE_TO_DESCEND, &message.getParam<Sensor>());
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE) != NULL)
			{
				stateMachine.changeState(EntityStateID::TURN);
			}
		}
	}

	void Fall::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(handleBodyCollisionMessage(message, Direction::BOTTOM))
		{
			stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void Walk::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ACTION_UP):
			{
				EntityStateID::Type defaultJumpStartState = EntityStateID::JUMP_START_45;
				stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP, &defaultJumpStartState);
				break;
			}
			case(MessageID::ENTER_STATE):
			case(MessageID::ACTION_FORWARD):
			{
				_stillWalking = true;
				break;
			}
			case(MessageID::UPDATE):
			{
				if(!_stillWalking)
				{
					stateMachine.changeState(EntityStateID::STAND);
				}
				else
				{
					sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(2.0f, 0.0f)));
					_stillWalking = false;
				}
				break;
			}
		}
	}

	void Turn::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			sendMessage(stateMachine, Message(MessageID::FLIP_ORIENTATION));
			stateMachine.changeState(EntityStateID::STAND);
		}
	}

	const float PrepareToJump::ANGLES_SIZE = 4;
	const float PrepareToJump::ANGLES[] = { toRadians(45.0f), toRadians(60.0f), toRadians(75.0f), toRadians(105.0) };
	const EntityStateID::Type PrepareToJump::JUMP_START_STATES[] = { EntityStateID::JUMP_START_45, EntityStateID::JUMP_START_60, EntityStateID::JUMP_START_75, EntityStateID::JUMP_START_105 };

	void PrepareToJump::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);

		if(message.getID() == MessageID::ENTER_STATE)
		{
			_jumpStartState = message.getParam<EntityStateID::Type>();
			_platformFound = false;
		}
		else if(isSensorMessage(message, SensorID::JUMP))
		{
			const Sensor& sensor = message.getParam<Sensor>();
			const Body& sensorOwner = sensor.getOwner();
			const Body* const sensedBody = sensor.getSensedBody();
			const float F = JUMP_FORCE;
			const float hangSensorSize = 20.0f; // TODO:
			Message getOrientation(MessageID::GET_ORIENTATION);
			sendMessage(stateMachine, getOrientation);
			Orientation::Type orientation = getOrientation.getParam<Orientation::Type>();
			float target = sensedBody->getBounds().getOppositeSide(orientation);
			float front = sensorOwner.getBounds().getSide(orientation);
			float x = (target - front) * orientation;
			
			if(x >= 0 && x < hangSensorSize - 1.0f)
			{
				_jumpStartState = EntityStateID::JUMP_START_90;
				_platformFound = true;
			}
			else
			{
				float max = 0.0f;
				float G = 1.0f;
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
						_platformFound = true;
					}
				}
			}
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			stateMachine.changeState(_jumpStartState, &_platformFound);
		}
		
	}

	void JumpStart::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ENTER_STATE):
			{
				_platformFound = message.getParam<bool>();
				break;
			}
			case(MessageID::ANIMATION_ENDED):
			{	
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(JUMP_FORCE*cos(_angle), JUMP_FORCE*sin(_angle))));
				stateMachine.changeState(_jumpState);
				break;
			}
			case(MessageID::ACTION_FORWARD):	
			{
				if(_angle != toRadians(45) && !_platformFound)
					stateMachine.changeState(EntityStateID::JUMP_START_45, &_platformFound);
				break;
			}
			case(MessageID::ACTION_BACKWARD):
			{
				stateMachine.changeState(EntityStateID::TURN);
				break;
			}
		}
	}

	void JumpUp::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(handleBodyCollisionMessage(message, Direction::TOP))
			stateMachine.changeState(EntityStateID::STAND);
		else if(message.getID() == MessageID::UPDATE)
			update(stateMachine);
	}

	void JumpUp::update(EntityStateMachine& stateMachine)
	{
		Message getForce(MessageID::GET_FORCE);
		sendMessage(stateMachine, getForce);
		const Vector& force = getForce.getParam<Vector>();
		if(force.getY() <= 0.0f)
			stateMachine.changeState(EntityStateID::STAND);
	}

	void JumpForward::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(handleBodyCollisionMessage(message, Direction::BOTTOM))
		{
			stateMachine.changeState(EntityStateID::JUMP_FORWARD_END);
		}
	}

	void JumpForwardEnd::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			stateMachine.changeState(EntityStateID::STAND);
		}
	}

	void Hanging::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Sensor& sensor = message.getParam<Sensor>();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			const Rect& personBounds = _person->getBounds();
			const Rect& platformBounds = _platform->getBounds();
			float platformTop = platformBounds.getTop();
			float entityTop = personBounds.getTop();
			float moveY = platformTop - entityTop;

			Message getOrientation(MessageID::GET_ORIENTATION);
			sendMessage(stateMachine, getOrientation);
			Orientation::Type orientation = getOrientation.getParam<Orientation::Type>();
			float platformFront = platformBounds.getOppositeSide(orientation);
			float entityFront = personBounds.getSide(orientation);
			float moveX = (platformFront - entityFront) * orientation;
			sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(moveX, moveY)));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			stateMachine.changeState(EntityStateID::HANG);
		}
	}

	void Hang::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ACTION_DOWN):
			{	
				stateMachine.changeState(EntityStateID::DROP);
				break;
			}
			case(MessageID::ACTION_UP):
			{	
				stateMachine.changeState(EntityStateID::CLIMBE);
				break;
			}
		}
	}

	void Drop::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(isSensorMessage(message, SensorID::HANG))
		{
			_platformFound = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = false;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_platformFound)
				stateMachine.changeState(EntityStateID::STAND);
			else
				_platformFound = false;

		}
	}

	void Climbe::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ENTER_STATE):
			{
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector::Zero));
				_moved = false;
				break;
			}
			case(MessageID::ANIMATION_ENDED):
			{
				Message getSize(MessageID::GET_SIZE);
				sendMessage(stateMachine, getSize);
				const Vector& size = getSize.getParam<Vector>();
				float forceX = 1.0f;
				float forceY = size.getHeight() - 1.0f;
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
				_moved = true;
				break;
			}
			case(MessageID::UPDATE):
			{
				if(_moved)
				{
					stateMachine.changeState(EntityStateID::STAND);
				}
				break;
			}
		}
	}

	void PrepareToDescend::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Sensor& sensor = message.getParam<Sensor>();
			_person = &sensor.getOwner();
			_platform = sensor.getSensedBody();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			Message getOrientation(MessageID::GET_ORIENTATION);
			sendMessage(stateMachine, getOrientation);
			Orientation::Type orientation = getOrientation.getParam<Orientation::Type>();
			const Rect& personBounds = _person->getBounds();
			const Rect& platformBounds = _platform->getBounds();
			float platformEdge = platformBounds.getOppositeSide(orientation) + 1.0f * orientation;
			float entityFront = personBounds.getSide(orientation);
			float moveX = (platformEdge - entityFront) * orientation;
			if(moveX != 0.0f)
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(moveX, 0.0f)));
			else
				stateMachine.changeState(EntityStateID::DESCEND);
		}
	}

	void Descend::enter(EntityStateMachine& stateMachine)
	{
		Message getSize(MessageID::GET_SIZE);
		sendMessage(stateMachine, getSize);
		const Vector& size = getSize.getParam<Vector>();
		float forceX = -1.0f;
		float forceY = -(size.getHeight() - 1.0f);
		sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(forceX, forceY)));
	}

	void Descend::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ENTER_STATE):
			{
				enter(stateMachine);
				break;;
			}
			case(MessageID::ANIMATION_ENDED):
			{
				stateMachine.changeState(EntityStateID::HANG);
				break;
			}
			case(MessageID::UPDATE):
			{
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector::Zero));
				break;
			}
		}
	}

}