#include "ActionController.h"
#include "MessageUtils.h"
#include "Serialization.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "DynamicBody.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"
#include "Grid.h"
#include "JumpInfoByTypeProvider.h"
#include "Ids.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Constants
	 *********************************************************************************************/
	const Hash ActionController::TYPE = Hash("action-controller");
	float ActionController::WALK_ACC_PER_SECOND(1250.0f);
	float ActionController::JUMP_FORCE_PER_SECOND(400.0f);
	float ActionController::FALL_ALLOW_JUMP_TIME(0.1f);
	float ActionController::JUMP_STOP_MODIFIER(0.5f);
	float ActionController::MAX_WALK_JUMP_MODIFIER(1.15f);

	/**********************************************************************************************
	 * Helpers
	 *********************************************************************************************/
	ActionController& ActionController::getActionController(StateMachineComponent* stateMachine)
	{
		return *static_cast<ActionController*>(stateMachine);
	}

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	ActionController::ActionController(float maxWalkForcePerSecond, Hash initialStateId) :
		StateMachineComponent(getStates(), "ACT", initialStateId), _ledgeDetector(SensorIds::LEDGE_SENSOR_ID, *this), _handleMessageHelper(*this), MAX_WALK_FORCE_PER_SECOND(maxWalkForcePerSecond), _isInvestigated(false) {}

	void ActionController::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::GET_DRAGGABLE)
		{
			if (getCurrentStateID() != ActionControllerStateIds::DRAG_STAND_STATE &&
				getCurrentStateID() != ActionControllerStateIds::DRAG_FORWARD_STATE &&
				getCurrentStateID() != ActionControllerStateIds::DRAG_BACKWARDS_STATE)
			{
				_draggableId = Hash::INVALID;
			}
			message.setParam(&_draggableId);
		}
		else if (message.getID() == MessageID::INVESTIGATE)
		{
			_isInvestigated = true;
		}
		else if (message.getID() == MessageID::IS_INVESTIGATED)
		{
			message.setParam(&_isInvestigated);
		}
		_ledgeDetector.handleMessage(message);
		StateMachineComponent::handleMessage(message);
		_handleMessageHelper.handleMessage(message);
	}

	HashStateMap ActionController::getStates() const
	{
		using namespace ActionControllerStates;
		HashStateMap states;
		states[ActionControllerStateIds::STAND_STATE] = new Stand();
		states[ActionControllerStateIds::FALL_STATE] = new Fall();
		states[ActionControllerStateIds::WALK_STATE] = new Walk();
		states[ActionControllerStateIds::TURN_STATE] = new Turn();
		states[ActionControllerStateIds::JUMP_STATE] = new Jump();
		states[ActionControllerStateIds::JUMP_END_STATE] = new JumpEnd();
		states[ActionControllerStateIds::HANG_STATE] = new Hang();
		states[ActionControllerStateIds::CLIMB_STATE] = new Climb();
		states[ActionControllerStateIds::DESCEND_STATE] = new Descend();
		states[ActionControllerStateIds::TAKEDOWN_STATE] = new Takedown();
		states[ActionControllerStateIds::DIE_STATE] = new Die();
		states[ActionControllerStateIds::DEAD_STATE] = new Dead();
		states[ActionControllerStateIds::AIM_STATE] = new Aim();
		states[ActionControllerStateIds::FIRE_STATE] = new Fire();
		states[ActionControllerStateIds::DRAG_STAND_STATE] = new DragStand();
		states[ActionControllerStateIds::DRAG_FORWARD_STATE] = new DragForward();
		states[ActionControllerStateIds::DRAG_BACKWARDS_STATE] = new DragBackwards();
		states[ActionControllerStateIds::INVESTIGATE_STATE] = new Investigate();
		return states;
	}

	Hash ActionController::getInitialState() const { return ActionControllerStateIds::FALL_STATE; }

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/

	namespace ActionControllerStates
	{
		void Stand::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void Stand::handleMessage(Message& message)
		{
			if(ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::WALK_STATE);
			}
			else if(message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::TURN_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_START)
			{
				JumpInfoByTypeProvider helper(JumpType::UP);
				_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
			}
		}

		void Fall::enter(void* param)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FALL_ANIMATION)));
		}

		void Fall::handleMessage(Message& message)
		{
			// TempFlag 1 - want to hang
			if(ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_UP_START)
			{
				// Need to be in start jump start, because can occur on fall from ledge
				if(_stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
				{
					JumpInfoByTypeProvider helper(JumpType::FORWARD);
					_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
				}
			}
		}

		void Walk::enter(void* param)
		{
			// TempFlag 1 - still walking
			_stateMachine->setFrameFlag1(true);

			// State flag - started animation
			_stateMachine->setStateFlag(false);

			// BRODER
			Vector force = Vector(32.0f, 0.0f);
			_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
		}

		void Walk::handleMessage(Message& message)
		{
			if(ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_UP_START)
			{
				JumpInfoByTypeProvider helper(JumpType::FORWARD);
				_stateMachine->changeState(ActionControllerStateIds::JUMP_STATE, &helper);
			}
			// TempFlag 1 - still walking
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_INVESTIGATE)
			{
				_stateMachine->changeState(ActionControllerStateIds::INVESTIGATE_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (!_stateMachine->getStateFlag())
				{
					_stateMachine->setStateFlag(true);
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION)));
				}
				if (!_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
				}
				else
				{
					// We need to apply this every update because the ground has infinite restitution. 
					float x = ActionController::WALK_ACC_PER_SECOND * _stateMachine->getTimer().getElapsedTime();
					if (x > ActionController::getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND)
						x = ActionController::getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND;
					Vector force = Vector(x, 0.0f);
					_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
				}
			}
			else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if (sameSign(side, collision.getX()) && !AngleUtils::radian().isSteep(collision.getAngle()))
				{
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
				}
			}
		}

		void Turn::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::TURN_ANIMATION)));
		}

		void Turn::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
				_stateMachine->changeState(ActionControllerStateIds::WALK_STATE);
			}
		}

		void Jump::enter(void* param)
		{
			const JumpInfoByTypeProvider& jumpHelper = *static_cast<JumpInfoByTypeProvider*>(param);
			float angle = jumpHelper.getInfo().getAngle();
			Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
			float force = ActionController::JUMP_FORCE_PER_SECOND + velocity.getLength();
			float max = ActionController::JUMP_FORCE_PER_SECOND * ActionController::MAX_WALK_JUMP_MODIFIER;
			if(force > max)
				force = max;
			float jumpForceX = force * cos(angle);
			float jumpForceY = force * sin(angle);
			Vector jumpVector = Vector(jumpForceX, jumpForceY);
			_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &jumpVector));
			Hash animation = jumpHelper.getInfo().getJumpAnimation();
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(animation)));

			// Pressed on transition
			_stateMachine->setFrameFlag1(true);
		}

		void Jump::handleMessage(Message& message)
		{
			// TempFlag 1 - Want to hang
			if(ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::SENSOR_HANG_UP)
			{
				if(_stateMachine->getFrameFlag1())
					_stateMachine->changeState(ActionControllerStateIds::HANG_STATE, message.getParam());
			}
			else if(message.getID() == MessageID::SENSOR_HANG_FRONT)
			{
				_stateMachine->changeState(ActionControllerStateIds::CLIMB_STATE, message.getParam());
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				// StateFlag 1 - stopped jumping
				if (!_stateMachine->getStateFlag() && !_stateMachine->getFrameFlag1())
				{
					Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
					if(velocity.getY() > 0.0f)
					{
						velocity.setY(velocity.getY() * ActionController::JUMP_STOP_MODIFIER);
						_stateMachine->setStateFlag(true);
					}
				}
			}
		}

		

		void JumpEnd::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void JumpEnd::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				// BRODER
				if (_stateMachine->getTimer().getElapsedTime() > 0.1f)
					_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}

		void Hang::enter(void* param)
		{
			bool bodyEnabled = false; 
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
			if(param)
				_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::HANG_ANIMATION)));
		}

		void Hang::exit(void* param)
		{
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_TANGIBLE, &bodyEnabled));
		}

		void Hang::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ACTION_DOWN)
			{	
				_stateMachine->changeState(ActionControllerStateIds::FALL_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{	
				_stateMachine->changeState(ActionControllerStateIds::CLIMB_STATE);
			}
		}

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
			if(!param)
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
			if(message.getID() == MessageID::UPDATE)
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
			else if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition + ActionController::getActionController(_stateMachine).getHangDescendMovement();
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}

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
			if(message.getID() == MessageID::UPDATE)
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
			else if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				const OBBAABBWrapper size(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
				Vector position = getPosition(*_stateMachine);
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					ActionController::getActionController(_stateMachine).getHangDescendGroundDelta();
				position.setY(startPosition.getY() - size.getHeight());
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(ActionControllerStateIds::HANG_STATE);
			}
		}

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
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}

		void Die::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DIE_ANIMATION)));
			int category = CollisionCategory::DEAD | CollisionCategory::DRAGGABLE;
			_stateMachine->raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &category));
			int dynamicObstacleCollisionCategory = CollisionCategory::DYNAMIC_OBSTACLE;
			_stateMachine->raiseMessage(Message(MessageID::REMOVE_FROM_COLLISION_MASK, &dynamicObstacleCollisionCategory));
		}

		void Die::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				float normalizedTime;
				_stateMachine->raiseMessage(Message(MessageID::GET_ANIMATION_NORMALIZED_TIME, &normalizedTime));
				DynamicBody& body = *static_cast<DynamicBody*>(_stateMachine->getEntity().get(DynamicBody::TYPE));
				body.getFixture().getLocalShape().setRadius(Vector(body.getFixture().getLocalShape().getRadiusY()  *normalizedTime, body.getFixture().getLocalShape().getRadiusY()));
				_stateMachine->getEntity().getManager().getGameState().getGrid().update(&body.getFixture());			
			}
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::DEAD_STATE);
			}
		}

		void Dead::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::DEAD_ANIMATION)));
			
		}

		void Aim::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
		}

		void Aim::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_FIRE)
			{
				// Temp flag 1 - Firing
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::ACTION_HOLSTER)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				RayCastResult& result = *static_cast<RayCastResult*>(message.getParam());
				if (!result.getFixture().getCategory() == CollisionCategory::PLAYER)
					return;

				const Vector& sourcePosition = getPosition(_stateMachine->getEntity());
				const Vector& targetPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(result.getFixture().getEntityId(), Message(MessageID::GET_POSITION)));
				Vector vector = targetPosition - sourcePosition;

				Side::Enum sourceSide = getOrientation(_stateMachine->getEntity());
				if (sourceSide == Side::LEFT)
					vector.setX(-vector.getX());

				float direction = vector.getAngle();
				float weight = direction / PI + 0.5f;
				if(_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(ActionControllerStateIds::FIRE_STATE, &weight);
				}
				else
				{
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_DOWN_ANIMATION, false, 0.0f, 1)));
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::AIM_UP_ANIMATION, false, 0.0f, 2, weight)));
				}
			}
		}

		static const Hash PLAYER("ENT_PLAYER");

		void Fire::enter(void* param)
		{
			float weight = getFloatParam(param);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FIRE_DOWN_ANIMATION, false, 0.0f, 1)));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::FIRE_UP_ANIMATION, false, 0.0f, 2, weight)));
			_stateMachine->getEntity().getManager().sendMessageToEntity(Hash(PLAYER), Message(MessageID::DIE));
		}

		void Fire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}

		void DragStand::enter(void* param)
		{
			if (param)
				ActionController::getActionController(_stateMachine).setDraggableId(getHashParam(param));

			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::STAND_ANIMATION)));
			ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
		}

		void DragStand::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{

			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_FORWARD_STATE);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_BACKWARDS_STATE);
			}
		}

		void DragForward::enter(void* param)
		{
			// Frame flag 1 - Still walking forward
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION)));
			ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragForward::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_BACKWARDS_STATE);
			}
		}

		void DragBackwards::enter(void* param)
		{
			// Frame flag 1 - Still walking backwards
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::WALK_ANIMATION, true)));
			ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragBackwards::handleMessage(Message& message)
		{
			if (ActionController::getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(ActionControllerStateIds::DRAG_FORWARD_STATE); 
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
		}

		void Investigate::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AnimationIds::INVESTIGATE_ANIMATION)));
		}

		void Investigate::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(ActionControllerStateIds::STAND_STATE);
			}
		}
	}
}
