#include "EntityStates.h"
#include <Temporal/Physics/Sensor.h>
#include <math.h>

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
				stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP);
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
				stateMachine.changeState(EntityStateID::PREPARE_TO_DESCEND);
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
				stateMachine.changeState(EntityStateID::PREPARE_TO_JUMP);
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

		// TODO: Handle default
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_jumpStartState = EntityStateID::JUMP_START_90;
		}
		else if(isSensorMessage(message, SensorID::JUMP))
		{
			const float F = JUMP_FORCE;
			const Sensor& sensor = message.getParam<Sensor>();
			const Body& sensorOwner = sensor.getOwner();
			const Body* const sensedBody = sensor.getSensedBody();
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
					}
				}
			}
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			stateMachine.changeState(_jumpStartState);
		}
		
	}

	void JumpStart::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ANIMATION_ENDED):
			{	
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector(JUMP_FORCE*cos(_angle), JUMP_FORCE*sin(_angle))));
				stateMachine.changeState(_jumpState);
				break;
			}
			case(MessageID::ACTION_FORWARD):	
			{
				/*if(_angle != toRadians(45) && !(stateMachine.getBody().getSensor(stateMachine.JUMP_SENSOR).isSensing()))
					stateMachine.changeState(EntityStateID::JUMP_START_45);*/
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
		/*
		ENTER = _platform = &stateMachine.getBody().getSensor(stateMachine.HANG_SENSOR).getSensedBody()->getBounds();
		stateMachine.getBody().setForce(Vector::Zero);
		float platformTop = _platform->getTop();
		float entityTop = stateMachine.getBody().getBounds().getTop();
		float moveY = platformTop - entityTop;

		Orientation::Type orientation = stateMachine.getBody().getOrientation();
		float platformFront = _platform->getOppositeSide(orientation);
		float entityFront = stateMachine.getBody().getFront();
		float moveX = (platformFront - entityFront) * orientation;

		if(moveX != 0 || moveY != 0)
			stateMachine.getBody().setForce(Vector(moveX, moveY));
		else if(stateMachine.getAnimator().isEnded())
			stateMachine.changeState(EntityStateID::HANG);*/
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
		/*if(!stateMachine.getBody().getSensor(stateMachine.HANG_SENSOR).isSensing())
		{
			stateMachine.changeState(EntityStateID::STAND);
		}*/
	}

	void Climbe::enter(EntityStateMachine& stateMachine)
	{
		/*EntityState::enter(stateMachine);
		float drawCenterX = EntityState::getDrawCenter(stateMachine).getX();
		float drawCenterY = stateMachine.getBody().getBounds().getTop();
		_drawCenter = Vector(drawCenterX, drawCenterY);
		float forceX = 1.0f;
		float forceY = stateMachine.getBody().getBounds().getHeight() - 1.0f;
		stateMachine.getBody().setForce(Vector(forceX, forceY));*/
	}

	void Climbe::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		EntityState::handleMessage(stateMachine, message);
		switch(message.getID())
		{
			case(MessageID::ENTER_STATE):
			{
				enter(stateMachine);
				break;
			}
			case(MessageID::ANIMATION_ENDED):
			{
				stateMachine.changeState(EntityStateID::STAND);
				break;
			}
			case(MessageID::UPDATE):
			{
				sendMessage(stateMachine, Message(MessageID::SET_FORCE, &Vector::Zero));
				break;
			}
		}
		
	}

	void PrepareToDescend::handleMessage(EntityStateMachine& stateMachine, Message& message)
	{
		
		/*ENTER = Orientation::Type orientation = stateMachine.getBody().getOrientation();
		_platformEdge = stateMachine.getBody().getSensor(stateMachine.BACK_EDGE_SENSOR).getSensedBody()->getBounds().getOppositeSide(orientation) + 1.0f * orientation;

		EntityState::handleMessage(stateMachine, message);
		Orientation::Type orientation = stateMachine.getBody().getOrientation();
		float entityFront = stateMachine.getBody().getFront();
		float moveX = (_platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
			stateMachine.getBody().setForce(Vector(moveX, 0.0f));
		else
			stateMachine.changeState(EntityStateID::DESCEND);*/
	}

	void Descend::enter(EntityStateMachine& stateMachine)
	{
		/*float drawCenterX = EntityState::getDrawCenter(stateMachine).getX();
		float drawCenterY = stateMachine.getBody().getBounds().getBottom();
		_drawCenter = Vector(drawCenterX, drawCenterY);
		float forceX = -1.0f;
		float forceY = -(stateMachine.getBody().getBounds().getHeight() - 1.0f);
		stateMachine.getBody().setForce(Vector(forceX, forceY));*/
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