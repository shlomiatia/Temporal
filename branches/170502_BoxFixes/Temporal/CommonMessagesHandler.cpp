#include "CommonMessagesHandler.h"
#include "MessageUtils.h"
#include "Ids.h"
#include "Grid.h"
#include "PhysicsEnums.h"
#include "Math.h"
#include "ActionController.h"

namespace Temporal
{
	bool CommonMessagesHandler::handleStandWalkMessage(Message& message)
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
				_controller.changeState(ActionControllerStateIds::DESCEND_STATE, message.getParam());
		}
		else if (message.getID() == MessageID::ACTION_ACTIVATE)
		{
			int sourceCollisionGroup = getIntParam(_controller.raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			OBB shape = getShape(_controller);
			Side::Enum side = getOrientation(_controller);
			shape.setCenterX(shape.getCenterX() + (shape.getRadiusX() + 1.0f) * side);
			shape.setRadiusX(1.0f);
			shape.setRadiusY(shape.getRadiusY() - 1.0f);
			FixtureList result;
			_controller.getEntity().getManager().getGameState().getGrid().iterateTiles(shape, CollisionCategory::DRAGGABLE, sourceCollisionGroup, &result);
			if (result.size() != 0)
			{
				Hash draggable = result.at(0)->getEntityId();
				_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE, &draggable);
			}
			else
			{
				_isActivating = true;
			}
		}
		else if (message.getID() == MessageID::ACTION_TAKEDOWN)
		{
			_isTakingDown = true;
		}
		else if (message.getID() == MessageID::ACTION_AIM)
		{
			_controller.changeState(ActionControllerStateIds::AIM_STATE);
		}
		else if (message.getID() == MessageID::SENSOR_SENSE)
		{
			SensorParams& params = getSensorParams(message.getParam());
			if (_isActivating && params.getSensorId() == SensorIds::ACTIVATE_SENSOR_ID && params.getContact().getTarget().getCategory() == CollisionCategory::BUTTON)
			{
				Hash entityId = params.getContact().getTarget().getEntityId();
				_controller.getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::ACTIVATE));
				params.setHandled(true);
			}
			else if (_isTakingDown && params.getSensorId() == SensorIds::TAKEDOWN_SENSOR_ID)
			{
				void* isCovered = _controller.raiseMessage(Message(MessageID::IS_COVERED));
				if (!isCovered || !*static_cast<bool*>(isCovered))
				{
					Hash entityId = params.getContact().getTarget().getEntityId();
					_controller.changeState(ActionControllerStateIds::TAKEDOWN_STATE, &entityId);
				}
			}
		}
		else
		{
			return false;
		}
		return true;
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
		else if (message.getID() == MessageID::UPDATE)
		{
			Hash draggableId = ActionController::getActionController(&_controller).getDraggableId();
			const Vector& velocity = *static_cast<const Vector*>(_controller.getEntity().getManager().sendMessageToEntity(draggableId, Message(MessageID::GET_VELOCITY)));
			if (velocity.getY() != 0.0f)
			{
				_controller.changeState(ActionControllerStateIds::STAND_STATE);
				return true;
			}
			else if (_controller.getCurrentStateID() != ActionControllerStateIds::DRAG_STAND_STATE)
			{
				if (!_controller.getFrameFlag1())
				{
					_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE);
					return true;
				}
				else
				{
					Vector movement = Vector(ActionController::getActionController(&_controller).MAX_WALK_FORCE_PER_SECOND / 2.0f, 0.0f);
					if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_BACKWARDS_STATE)
						movement.setX(movement.getX() * -1.0f);


					_controller.raiseMessage(Message(MessageID::SET_IMPULSE, &movement));

					Side::Enum sourceSide = getOrientation(_controller);
					Side::Enum targetSide = *static_cast<Side::Enum*>(_controller.getEntity().getManager().sendMessageToEntity(draggableId, Message(MessageID::GET_ORIENTATION)));
					movement.setX(movement.getX() * sourceSide * targetSide);

					_controller.getEntity().getManager().sendMessageToEntity(draggableId, Message(MessageID::SET_IMPULSE, &movement));
				}
			}
		}
		return false;
	}

	void CommonMessagesHandler::handleDragEnter()
	{
		Hash draggableId = ActionController::getActionController(&_controller).getDraggableId();
		if (_controller.getEntity().getManager().getEntity(draggableId)->get(ActionController::TYPE))
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DRAG_ANIMATION, false, 0.0f, 1)));
		}
		else
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_DOWN_ANIMATION, false, 0.0f, 1)));
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_UP_ANIMATION, false, 0.0f, 2, 0.5f)));
		}
	}

	void CommonMessagesHandler::die()
	{
		if (_controller.getCurrentStateID() != ActionControllerStateIds::DIE_STATE)
		{
			_controller.changeState(ActionControllerStateIds::DIE_STATE);
		}
	}

	void CommonMessagesHandler::handleMessage(Message& message)
	{

		if (message.getID() == MessageID::DIE)
		{
			die();
		}
		else if (message.getID() == MessageID::COLLIDED_BY_ENTITY)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if (collision.getY() > 0.0f)
			{
				die();
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			_isDescending = _isActivating = _isTakingDown = false;
		}
		else if (message.getID() == MessageID::LOST_GROUND)
		{
			if (_controller.getCurrentStateID() != ActionControllerStateIds::FALL_STATE &&
				_controller.getCurrentStateID() != ActionControllerStateIds::JUMP_STATE)
			{
				if (_controller.getCurrentStateID() == ActionControllerStateIds::DESCEND_STATE)
				{
					bool bodyEnabled = true;
					_controller.raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
					_controller.raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
				}
				_controller.changeState(ActionControllerStateIds::FALL_STATE);
			}
		}
	}
}
