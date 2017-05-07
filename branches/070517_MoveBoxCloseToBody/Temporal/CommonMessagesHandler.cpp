#include "CommonMessagesHandler.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "Grid.h"
#include "PhysicsEnums.h"
#include "Math.h"
#include "ActionController.h"

namespace Temporal
{
	bool CommonMessagesHandler::handleStandWalkAimMessage(Message& message)
	{
		if (message.getID() == MessageID::CAN_ATTACK)
		{
			bool* canAttack = static_cast<bool*>(message.getParam());
			*canAttack = true;
		}
		else if (message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if (message.getID() == MessageID::SENSOR_DESCEND)
		{
			if (_isDescending)
			{
				_controller.changeState(ActionControllerStateIds::DESCEND_STATE, message.getParam());
				_isDescending = false;
				return true;
			}
		}
		else if (message.getID() == MessageID::ACTION_ACTIVATE)
		{
			_isActivating = true;
		}
		else if (message.getID() == MessageID::ACTION_TAKEDOWN)
		{
			_isTakingDown = true;
		}
		else if (message.getID() == MessageID::ACTION_AIM)
		{
			_controller.changeState(ActionControllerStateIds::AIM_STATE);
			return true;
		}
		else if (message.getID() == MessageID::SENSOR_SENSE)
		{
			if (handleSensorSense(message))
			{
				return true;
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			_isDescending = _isActivating = _isTakingDown = false;
		}
		return false;
	}

	bool CommonMessagesHandler::handleSensorSense(Message& message)
	{
		SensorParams& params = getSensorParams(message.getParam());
		if (_isActivating && params.getSensorId() == SensorIds::ACTIVATE_SENSOR_ID)
		{
			if (activate(params))
			{
				return true;
			}
		}
		else if (_isTakingDown && params.getSensorId() == SensorIds::TAKEDOWN_SENSOR_ID)
		{
			if (takedown(params))
			{
				return true;
			}
		}
		return false;
	}

	bool CommonMessagesHandler::activate(SensorParams& params)
	{
		params.setHandled(true);
		_isActivating = false;
		Hash entityId = params.getContact().getTarget().getEntityId();
		if (params.getContact().getTarget().getCategory() == CollisionCategory::BUTTON)
		{
			_controller.getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::ACTIVATE));
		}
		else
		{
			_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE, &entityId);
			return true;
		}
		return false;
	}

	bool CommonMessagesHandler::takedown(SensorParams& params)
	{
		params.setHandled(true);
		void* isCovered = _controller.raiseMessage(Message(MessageID::IS_COVERED));
		if (!isCovered || !*static_cast<bool*>(isCovered))
		{
			_isTakingDown = false;
			Hash entityId = params.getContact().getTarget().getEntityId();
			_controller.changeState(ActionControllerStateIds::TAKEDOWN_STATE, &entityId);
			return true;
		}
		return false;
	}

	bool CommonMessagesHandler::handleFallJumpMessage(Message& message)
	{
		if (message.getID() == MessageID::COLLISIONS_CORRECTED)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if (collision.getY() < 0.0f && AngleUtils::radian().isModerate(collision.getRightNormal().getAngle()))
			{
				_controller.changeState(ActionControllerStateIds::JUMP_END_STATE);
				return true;
			}

		}
		return false;
	}

	bool CommonMessagesHandler::handleDragMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTION_ACTIVATE)
		{
			_controller.changeState(ActionControllerStateIds::STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::POST_LOAD)
		{
			_controller.changeState(ActionControllerStateIds::STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::TEMPORAL_ECHOS_MERGED)
		{
			_controller.changeState(ActionControllerStateIds::STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::GET_DRAGGABLE)
		{
			message.setParam(&_draggableId);
		}
		return false;
	}

	bool CommonMessagesHandler::handleDragWalkMessage(Message& message)
	{
		if (handleDragMessage(message))
		{
			return true;
		}
		else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
		{
			const Vector& collisions = getVectorParam(message.getParam());
			if (collisions.getX() != 0.0f)
			{
				_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE);
				return true;
			}

		}
		else if (message.getID() == MessageID::UPDATE)
		{
			const Vector& velocity = *static_cast<const Vector*>(_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::GET_VELOCITY)));
			if (velocity.getY() != 0.0f)
			{
				_controller.changeState(ActionControllerStateIds::STAND_STATE);
				return true;
			}

			if (!_controller.getFrameFlag1())
			{
				_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE);
				return true;
			}
				
			Vector& movement = getMovement();
			move(movement);
			moveDraggable(movement);
		}
		return false;
	}

	void CommonMessagesHandler::handleDragEnter()
	{
		Vector movement = getMovement();
		
		if (_controller.getEntity().getManager().getEntity(_draggableId)->get(ActionController::TYPE))
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DRAG_ANIMATION, false, 0.0f, 1)));
		}
		else
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_DOWN_ANIMATION, false, 0.0f, 1)));
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_UP_ANIMATION, false, 0.0f, 2, 0.5f)));
		}
	}

	void CommonMessagesHandler::handleDragWalkEnter()
	{
		handleDragEnter();
		Vector movement = getMovement();

		if (_controller.getEntity().getManager().getEntity(_draggableId)->get(ActionController::TYPE))
		{
			move(movement);
		}
		else
		{
			moveDraggable(movement);
		}
	}

	Vector CommonMessagesHandler::getMovement()
	{
		Vector movement = Vector(ActionController::getActionController(&_controller)._maxWalkForcePerSecond / 2.0f, 0.0f);
		if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_BACKWARDS_STATE)
		{
			movement.setX(movement.getX() * -1.0f);
		}
		return movement;
	}

	void CommonMessagesHandler::move(Vector& movement)
	{
		_controller.raiseMessage(Message(MessageID::SET_IMPULSE, &movement));
	}

	void CommonMessagesHandler::moveDraggable(Vector& movement)
	{
		Side::Enum sourceSide = getOrientation(_controller);
		Side::Enum targetSide = *static_cast<Side::Enum*>(_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::GET_ORIENTATION)));
		movement.setX(movement.getX() * sourceSide * targetSide);

		_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::SET_IMPULSE, &movement));
	}
}
