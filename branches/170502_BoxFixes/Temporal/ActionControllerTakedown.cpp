#include "ActionControllerTakedown.h"
#include "MessageUtils.h"
#include "Shapes.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Takedown::enter(void* param)
		{
			Hash takedownEntityId = getHashParam(param);
			const OBBAABBWrapper shape(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
			Entity* takedownEntity = _stateMachine->getEntity().getManager().getEntity(takedownEntityId);
			const OBBAABBWrapper takedownEntityShape(static_cast<OBB*>(takedownEntity->handleMessage(Message(MessageID::GET_SHAPE))));
			Side::Enum side = getOrientation(*_stateMachine);
			if ((takedownEntityShape.getLeft() - shape.getLeft()) * side < 0.0f)
			{
				side = Side::getOpposite(side);
				_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
			}
			float translationX = takedownEntityShape.getCenterX() - shape.getCenterX() - shape.getRadiusX() * 2.0f * side;
			_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &Vector(shape.getCenterX() + translationX, shape.getCenterY())));
			takedownEntity->handleMessage(Message(MessageID::DIE));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::TAKEDOWN_ANIMATION)));
		}

		void Takedown::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
