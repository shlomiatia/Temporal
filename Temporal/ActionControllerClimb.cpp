#include "ActionControllerClimb.h"
#include "SceneNode.h"
#include "ActionController.h"
#include "Ids.h"
#include "MessageUtils.h"

namespace Temporal
{
	namespace ActionControllerStates
	{
		void Climb::enter(void* param)
		{
			bool bodyEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));

			const SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			ActionController::getActionController(_stateMachine).setHangDescendOriginalTranslation(root.getTranslation());
			const OBBAABBWrapper shape(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
			float interpolation = 0.0f;
			Fixture* ground = 0;
			if (!param)
			{
				ActionController::getActionController(_stateMachine).setHangDescendMovement(Vector(0.0f, shape.getHeight()));
				interpolation = 0.0f;
				ground = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
			}
			else
			{
				_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));
				Vector movement = Vector(shape.getWidth() *  getOrientation(*_stateMachine), static_cast<Fixture*>(param)->getGlobalShape().getTop() - shape.getBottom());
				ActionController::getActionController(_stateMachine).setHangDescendMovement(movement);
				interpolation = 1.0f - (movement.getY() / shape.getHeight());
				ground = static_cast<Fixture*>(param);
			}

			Vector groundDelta = getPosition(*_stateMachine) - ground->getGlobalShape().getCenter();
			ActionController::getActionController(_stateMachine).setHangDescendGroundDelta(groundDelta);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DESCEND_ANIMATION, true, interpolation)));
		}

		void Climb::exit(void* param)
		{
			SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			root.setTranslation(ActionController::getActionController(_stateMachine).getHangDescendOriginalTranslation());
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
		}

		void Climb::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
				float normalizedTime;
				_stateMachine->raiseMessage(Message(MessageID::GET_ANIMATION_NORMALIZED_TIME, &normalizedTime));
				const Vector& movement = ActionController::getActionController(_stateMachine).getHangDescendMovement();
				Vector translation = normalizedTime * movement;
				Fixture* ground = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
				Vector startPosition = ground->getGlobalShape().getCenter() + ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition + translation;
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));

				root.setTranslation(ActionController::getActionController(_stateMachine).getHangDescendOriginalTranslation() + Vector(0.0f, movement.getY()) - Vector(0.0f, translation.getY()));
			}
			else if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition + ActionController::getActionController(_stateMachine).getHangDescendMovement();
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
