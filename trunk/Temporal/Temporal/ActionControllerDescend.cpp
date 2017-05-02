#include "ActionControllerDescend.h"
#include "MessageUtils.h"
#include "ActionController.h"
#include "SceneNode.h"
#include "Ids.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Descend::enter(void* param)
		{
			bool bodyEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));

			Vector groundDelta = getPosition(*_stateMachine) - static_cast<Fixture*>(param)->getGlobalShape().getCenter();
			ActionController::getActionController(_stateMachine).setHangDescendGroundDelta(groundDelta);

			const SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			ActionController::getActionController(_stateMachine).setHangDescendOriginalTranslation(root.getTranslation());

			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DESCEND_ANIMATION)));
		}

		void Descend::exit(void* param)
		{
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
			SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			root.setTranslation(ActionController::getActionController(_stateMachine).getHangDescendOriginalTranslation());
		}

		void Descend::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
				float normalizedTime;
				_stateMachine->raiseMessage(Message(MessageID::GET_ANIMATION_NORMALIZED_TIME, &normalizedTime));
				const OBBAABBWrapper size(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));

				float translation = normalizedTime * size.getHeight();
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition - Vector(0.0f, translation);
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				root.setTranslation(Vector(root.getTranslation().getX(), ActionController::getActionController(_stateMachine).getHangDescendOriginalTranslation().getY() + translation));
			}
			else if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				const OBBAABBWrapper size(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
				Vector position = getPosition(*_stateMachine);
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				position.setY(startPosition.getY() - size.getHeight());
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(ActionControllerStateIds::HANG_STATE);
			}
			else if (message.getID() == MessageID::LOST_GROUND)
			{
				bool bodyEnabled = true;
				_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
				_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
			}
		}
	}
}
