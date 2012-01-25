#include "EntityStates.h"
#include <Temporal\Physics\Physics.h>
#include <Temporal\Graphics\Graphics.h>
#include <math.h>
#include <sstream>

namespace Temporal
{
	void Stand::stateUpdate(Entity& entity)
	{
		if(entity.isMovingForward())
		{
			entity.changeState(EntityStateID::WALK);
		}
		else if(entity.isMovingBackward())
		{
			entity.changeState(EntityStateID::TURN);
		}
		else if(entity.getController().isUp())
		{
			entity.changeState(EntityStateID::PREPARE_TO_JUMP);
		}
		else if(entity.getController().isDown() && match(entity.getBody().getSensor(entity.BACK_EDGE_SENSOR).getSensedBodyDirection(), Direction::BOTTOM, Direction::BACK))
		{
			entity.changeState(EntityStateID::PREPARE_TO_DESCEND);
		}
		else if(entity.getController().isDown() && match(entity.getBody().getSensor(entity.FRONT_EDGE_SENSOR).getSensedBodyDirection(), Direction::BOTTOM, Direction::FRONT))
		{
			entity.changeState(EntityStateID::TURN);
		}
	}

	void Fall::stateUpdate(Entity& entity)
	{
		if(entity.getBody().getCollision() & Direction::BOTTOM)
		{
			entity.changeState(EntityStateID::STAND);
		}
	}

	void Walk::stateUpdate(Entity& entity)
	{
		if(entity.getController().isUp())
		{
			entity.changeState(EntityStateID::PREPARE_TO_JUMP);
		}
		else if(!entity.isMovingForward())
		{
			entity.changeState(EntityStateID::STAND);
		}
		else
		{
			entity.getBody().setForce(Vector(entity.WALK_FORCE, 0.0f)); 
		}
	}

	void Turn::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isEnded())
		{
			entity.getBody().flipOrientation();	
			entity.changeState(EntityStateID::STAND);
		}
	}

	const float PrepareToJump::ANGLES_SIZE = 4;
	const float PrepareToJump::ANGLES[] = { toRadians(45.0f), toRadians(60.0f), toRadians(75.0f), toRadians(105.0) };
	const EntityStateID::Type PrepareToJump::JUMP_START_STATES[] = { EntityStateID::JUMP_START_45, EntityStateID::JUMP_START_60, EntityStateID::JUMP_START_75, EntityStateID::JUMP_START_105 };

	void PrepareToJump::stateEnter(Entity& entity)
	{
		Body& body = entity.getBody();
		body.setForce(Vector::Zero);
		const Sensor& jumpSensor = body.getSensor(entity.JUMP_SENSOR);
		const float F = entity.JUMP_FORCE;
		
		EntityStateID::Type jumpStartState = entity.isMovingForward() ? EntityStateID::JUMP_START_45 : EntityStateID::JUMP_START_90;
		if(match(jumpSensor.getSensedBodyDirection(), Direction::BOTTOM, Direction::BACK))
		{
			const float hangSensorSize = body.getSensor(entity.HANG_SENSOR).getBounds().getWidth();
			const Body* const platform = jumpSensor.getSensedBody();
			Orientation::Type orientation = body.getOrientation();
			float target = platform->getBounds().getOppositeSide(orientation);
			float front = body.getFront();
			float x = (target - front) * orientation;
			
			if(x >= 0 && x < hangSensorSize - 1.0f)
			{
				jumpStartState = EntityStateID::JUMP_START_90;
			}
			else
			{
				float max = 0.0f;
				float G = Physics::GRAVITY;
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
						jumpStartState = JUMP_START_STATES[i];
					}
				}
			}
		}
		entity.changeState(jumpStartState);
	}

	void JumpStart::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isEnded())
		{	
			entity.getBody().setForce(Vector(entity.JUMP_FORCE*cos(_angle), entity.JUMP_FORCE*sin(_angle)));
			entity.changeState(_jumpState);
		}
		else if(entity.isMovingForward() && _angle != toRadians(45) && !(entity.getBody().getSensor(entity.JUMP_SENSOR).getSensedBodyDirection() & (Direction::BOTTOM | Direction::BACK)))
		{
			entity.changeState(EntityStateID::JUMP_START_45);
		}
	}

	void JumpUp::stateUpdate(Entity& entity)
	{
		if(entity.getBody().getCollision() & Direction::TOP || entity.getBody().getForce().getY() <= 0)
		{
			entity.changeState(EntityStateID::STAND);
		}
	}

	void JumpForward::stateUpdate(Entity& entity)
	{
		if(entity.getBody().getCollision() & Direction::BOTTOM)
		{
			entity.changeState(EntityStateID::JUMP_FORWARD_END);
		}
	}

	void JumpForwardEnd::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isEnded())
		{
			entity.changeState(EntityStateID::STAND);
		}
	}

	void Hanging::stateEnter(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		_platform = &entity.getBody().getSensor(entity.HANG_SENSOR).getSensedBody()->getBounds();
		entity.getSprite().reset(14, true);
	}

	void Hanging::stateUpdate(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		float platformTop = _platform->getTop();
		float entityTop = entity.getBody().getBounds().getTop();
		float moveY = platformTop - entityTop;

		Orientation::Type orientation = entity.getBody().getOrientation();
		float platformFront = _platform->getOppositeSide(orientation);
		float entityFront = entity.getBody().getFront();
		float moveX = (platformFront - entityFront) * orientation;

		if(moveX != 0 || moveY != 0)
			entity.getBody().setForce(Vector(moveX, moveY));
		else if(entity.getSprite().isEnded())
			entity.changeState(EntityStateID::HANG);
	}

	void Hang::stateUpdate(Entity& entity)
	{
		if(entity.getController().isDown())
		{
			entity.changeState(EntityStateID::DROP);
		}
		else if(entity.getController().isUp())
		{
			entity.changeState(EntityStateID::CLIMBE);
		}
	}

	void Drop::stateUpdate(Entity& entity)
	{
		if(!entity.getBody().getSensor(entity.HANG_SENSOR).getSensedBodyDirection() & (Direction::TOP | Direction::FRONT))
		{
			entity.changeState(EntityStateID::STAND);
		}
	}

	void Climbe::stateEnter(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		float drawCenterX = EntityState::getDrawCenter(entity).getX();
		float drawCenterY = entity.getBody().getBounds().getTop();
		_drawCenter = Vector(drawCenterX, drawCenterY);
		entity.getSprite().reset(8);
		_isFinished = false;
	}

	void Climbe::stateUpdate(Entity& entity)
	{
		if(_isFinished)
		{
			entity.changeState(EntityStateID::STAND);
		}
		else if (entity.getSprite().isEnded())
		{
			float forceX = 1.0f;
			float forceY = entity.getBody().getBounds().getHeight() - 1.0f;
			entity.getBody().setForce(Vector(forceX, forceY));
			_isFinished = true;
		}
	}

	void PrepareToDescend::stateEnter(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		Orientation::Type orientation = entity.getBody().getOrientation();
		_platformEdge = entity.getBody().getSensor(entity.BACK_EDGE_SENSOR).getSensedBody()->getBounds().getOppositeSide(orientation) + 1.0f * orientation;
	}

	void PrepareToDescend::stateUpdate(Entity& entity)
	{
		Orientation::Type orientation = entity.getBody().getOrientation();
		float entityFront = entity.getBody().getFront();
		float moveX = (_platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
			entity.getBody().setForce(Vector(moveX, 0.0f));
		else
			entity.changeState(EntityStateID::DESCEND);
	}

	void Descend::stateEnter(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		float drawCenterX = EntityState::getDrawCenter(entity).getX();
		float drawCenterY = entity.getBody().getBounds().getBottom();
		_drawCenter = Vector(drawCenterX, drawCenterY);
		entity.getSprite().reset(8, true);
		_isFinished = false;
	}

	void Descend::stateUpdate(Entity& entity)
	{
		if(_isFinished)
		{
			entity.changeState(EntityStateID::HANG);
		}
		else if (entity.getSprite().isEnded())
		{
			float forceX = -1.0f;
			float forceY = -(entity.getBody().getBounds().getHeight() - 1.0f);
			entity.getBody().setForce(Vector(forceX, forceY));
			_isFinished = true;
		}
	}
}