#include "DragMessagesHandler.h"
#include "Ids.h"
#include "MessageUtils.h"
#include "Math.h"
#include "ActionController.h"

namespace Temporal
{
	bool DragMessagesHandler::handleDragMessage(Message& message)
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


	bool DragMessagesHandler::handleDragWalkMessage(Message& message)
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
			if (handleDragWalkUpdate(message))
			{
				return true;
			}
		}
		return false;
	}

	bool DragMessagesHandler::handleDragWalkUpdate(Message& message)
	{
		if (checkIfStillWalking())
		{
			return true;
		}

		float framePeriod = getFloatParam(message.getParam());
		Vector originalDraggablePosition = getPosition(_draggableId);
		Vector movement = getMovement();

		if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_FORWARD_STATE)
		{
			moveDraggable(movement, framePeriod);
		}

		move(movement, framePeriod);

		if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_BACKWARDS_STATE)
		{
			moveDraggable(movement, framePeriod);
		}

		if (handleDistanceChange(originalDraggablePosition))
		{
			return true;
		}

		return false;
	}

	bool DragMessagesHandler::checkIfStillWalking()
	{
		if (!_controller.getFrameFlag1())
		{
			_controller.changeState(ActionControllerStateIds::DRAG_STAND_STATE);
			return true;
		}
		return false;
	}

	bool DragMessagesHandler::handleDistanceChange(Vector originalDraggablePosition)
	{
		if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_FORWARD_STATE ||
			_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_BACKWARDS_STATE ||
			_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_STAND_STATE)
		{
			Vector currentDistance = getPosition(_controller.getEntity()) - getPosition(_draggableId);
			if (!equals(_distance.getX(), currentDistance.getX()) || !equals(_distance.getY(), currentDistance.getY()))
			{
				if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_STAND_STATE)
				{
					_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::SET_POSITION, &originalDraggablePosition));
					_distance = getPosition(_controller.getEntity()) - getPosition(_draggableId);
				}
				else
				{
					_controller.changeState(ActionControllerStateIds::STAND_STATE);
					return true;
				}
			}
		}
		return false;
	}

	void DragMessagesHandler::handleDragEnter()
	{
		if (_controller.getEntity().getManager().getEntity(_draggableId)->get(ComponentsIds::ACTION_CONTROLLER))
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DRAG_ANIMATION, false, 0.0f, 1)));
		}
		else
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_DOWN_ANIMATION, false, 0.0f, 1)));
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_UP_ANIMATION, false, 0.0f, 2, 0.5f)));
		}
	}

	void DragMessagesHandler::setDraggableId(Hash draggableId)
	{
		_draggableId = draggableId;
		_distance = getPosition(_controller.getEntity()) - getPosition(_draggableId);
	}

	Vector DragMessagesHandler::getMovement()
	{
		Vector movement = Vector(ActionController::getActionController(&_controller)._maxWalkForcePerSecond / 2.0f, 0.0f);
		if (_controller.getCurrentStateID() == ActionControllerStateIds::DRAG_BACKWARDS_STATE)
		{
			movement.setX(movement.getX() * -1.0f);
		}
		return movement;
	}

	void DragMessagesHandler::move(Vector movement, float framePeriod)
	{
		_controller.raiseMessage(Message(MessageID::SET_IMPULSE, &movement));
		_controller.getEntity().get(ComponentsIds::DYNAMIC_BODY)->handleMessage(Message(MessageID::UPDATE, &framePeriod));
	}

	void DragMessagesHandler::moveDraggable(Vector movement, float framePeriod)
	{
		Side::Enum sourceSide = getOrientation(_controller);
		Side::Enum targetSide = *static_cast<Side::Enum*>(_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::GET_ORIENTATION)));
		movement.setX(movement.getX() * sourceSide * targetSide);

		_controller.getEntity().getManager().sendMessageToEntity(_draggableId, Message(MessageID::SET_IMPULSE, &movement));
		_controller.getEntity().getManager().getEntity(_draggableId)->get(ComponentsIds::DYNAMIC_BODY)->handleMessage(Message(MessageID::UPDATE, &framePeriod));
	}
}
