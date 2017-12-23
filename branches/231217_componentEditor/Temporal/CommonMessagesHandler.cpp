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
			_controller.getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::TOGGLE_ACTIVATION));
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
}
