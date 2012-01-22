#include "EntityStates.h"
#include <Temporal\Physics\Physics.h>
#include <Temporal\Graphics\Graphics.h>
#include <math.h>
#include <sstream>

namespace Temporal
{
	void Stand::stateUpdate(Entity& entity)
	{
		if(entity.getController().isUp())
		{
			entity.changeState(EntityStateID::JUMP_START);
		}
		else if(entity.isMovingForward())
		{
			entity.changeState(EntityStateID::WALK);
		}
		else if(entity.isMovingBackward())
		{
			entity.changeState(EntityStateID::TURN);
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
			entity.changeState(EntityStateID::JUMP_START);
		}
		else if(!entity.isMovingForward())
		{
			entity.changeState(EntityStateID::STAND);
		}
	}

	void Turn::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isAnimationEnded())
		{
			entity.getBody().flipOrientation();	
			entity.changeState(EntityStateID::STAND);
		}
	}

	const float JumpStart::ANGLES_SIZE = 3;
	const float JumpStart::ANGLES[] = {toRadians(45.0f), toRadians(60.0f), toRadians(75.0f) };

	void JumpStart::stateEnter(Entity& entity)
	{
		entity.getBody().setForce(Vector::Zero);
		const float defaultAngle = entity.isMovingForward() ? toRadians(45.0f) : toRadians(90.0f);
		const float F = entity.JUMP_FORCE;
		const Body& body = entity.getBody();
		// TODO: ID
		const float hangSensorSize = body.getSensor(entity.HANG_SENSOR).getBounds().getWidth();
		const Sensor& jumpSensor = body.getSensor(entity.JUMP_SENSOR);
		const Body* const platform = jumpSensor.getSensedBody();
		float angle;
		if(!match(jumpSensor.getSensedBodyDirection(), Direction::BOTTOM, Direction::BACK))
		{
			angle = defaultAngle;
		}
		else
		{
			Orientation::Type orientation = body.getOrientation();
			float target = platform->getBounds().getOppositeSide(orientation);
			float front = body.getFront();
			float x = (target - front) * orientation;
			if(x >= -hangSensorSize + 1.0f && x < 0.0f)
			{
				angle = toRadians(105.0f);
			}
			else if(x < hangSensorSize - 1.0f)
			{
				angle = toRadians(90.0f);
			}
			else
			{
				angle = defaultAngle;
				float max = 0.0f;
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
					float G = Physics::GRAVITY;
					float A = ANGLES[i];

					float y = (-G*pow(x, 2) + pow(F, 2)*sin(A)*cos(A)*x)/pow(F*cos(A), 2);
					if(max < y)
					{
						max = y;
						angle = A;
					}
				}
			}
		}
		_force = Vector(F*cos(angle), F*sin(angle));
		_isJumpingForward =  angle == toRadians(45) || angle == toRadians(60);
		if(_isJumpingForward)
			entity.getSprite().reset(9);
		else
			entity.getSprite().reset(5);
	}

	void JumpStart::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isAnimationEnded())
		{
			entity.getBody().setForce(_force);
			if(_isJumpingForward)
				entity.changeState(EntityStateID::JUMP_FORWARD);
			else
				entity.changeState(EntityStateID::JUMP_UP);
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
		if(entity.getBody().getForce().getY() <= 0 && entity.getBody().getCollision() & Direction::BOTTOM)
		{
			entity.changeState(EntityStateID::JUMP_FORWARD_END);
		}
	}

	void JumpForwardEnd::stateUpdate(Entity& entity)
	{
		if(entity.getSprite().isAnimationEnded())
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
		else if(entity.getSprite().isAnimationEnded())
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
		else if (entity.getSprite().isAnimationEnded())
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
		else if (entity.getSprite().isAnimationEnded())
		{
			float forceX = -1.0f;
			float forceY = -(entity.getBody().getBounds().getHeight() - 1.0f);
			entity.getBody().setForce(Vector(forceX, forceY));
			_isFinished = true;
		}
	}
}