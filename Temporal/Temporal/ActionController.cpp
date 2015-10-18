#include "ActionController.h"
#include "MovementUtils.h"
#include "MessageUtils.h"
#include "Serialization.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "DynamicBody.h"
#include "ShapeOperations.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"
#include "Grid.h"

#include "TemporalPeriod.h"

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

	static const Hash LEDGE_SENSOR_ID = Hash("SNS_LEDGE");
	static const Hash ACTIVATE_SENSOR_ID = Hash("SNS_ACTIVATE");
	static const Hash TAKEDOWN_SENSOR_ID = Hash("SNS_TAKEDOWN");

	static Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
	static Hash TURN_ANIMATION = Hash("POP_ANM_TURN");
	static Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
	static Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP2");
	static Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
	static Hash DESCEND_ANIMATION = Hash("POP_ANM_DESCEND");
	static Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP");
	static Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_STAND");
	static Hash WALK_ANIMATION = Hash("POP_ANM_WALK");
	static Hash TAKEDOWN_ANIMATION = Hash("POP_ANM_TAKEDOWN");
	static Hash DIE_ANIMATION = Hash("POP_ANM_DIE");
	static Hash DEAD_ANIMATION = Hash("POP_ANM_DEAD");
	static Hash AIM_DOWN_ANIMATION = Hash("POP_ANM_AIM_DOWN");
	static Hash AIM_UP_ANIMATION = Hash("POP_ANM_AIM_UP");
	static Hash FIRE_DOWN_ANIMATION = Hash("POP_ANM_FIRE_DOWN");
	static Hash FIRE_UP_ANIMATION = Hash("POP_ANM_FIRE_UP");
	static Hash INVESTIGATE_ANIMATION = Hash("POP_ANM_INVESTIGATE");
	static Hash DRAG_ANIMATION = Hash("POP_ANM_DRAG");

	static const Hash STAND_STATE = Hash("ACT_STT_STAND");
	static const Hash FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash WALK_STATE = Hash("ACT_STT_WALK");
	static const Hash TURN_STATE = Hash("ACT_STT_TURN");
	static const Hash JUMP_STATE = Hash("ACT_STT_JUMP");
	static const Hash JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
	static const Hash HANG_STATE = Hash("ACT_STT_HANG");
	static const Hash CLIMB_STATE = Hash("ACT_STT_CLIMB");
	static const Hash DESCEND_STATE = Hash("ACT_STT_DESCEND");
	static const Hash TAKEDOWN_STATE = Hash("ACT_STT_TAKEDOWN");
	static const Hash DIE_STATE = Hash("ACT_STT_DIE");
	static const Hash DEAD_STATE = Hash("ACT_STT_DEAD");
	static const Hash AIM_STATE = Hash("ACT_STT_AIM");
	static const Hash FIRE_STATE = Hash("ACT_STT_FIRE");
	static const Hash DRAG_STAND_STATE = Hash("ACT_STT_DRAG_STAND");
	static const Hash DRAG_FORWARD_STATE = Hash("ACT_STT_DRAG_FORWARD");
	static const Hash DRAG_BACKWARDS_STATE = Hash("ACT_STT_DRAG_BACKWARDS");
	static const Hash INVESTIGATE_STATE = Hash("ACT_STT_INVESTIGATE");

	/**********************************************************************************************
	 * Helpers
	 *********************************************************************************************/
	ActionController& getActionController(StateMachineComponent* stateMachine)
	{
		return *static_cast<ActionController*>(stateMachine);
	}

	const JumpInfo JumpHelper::JUMP_UP_INFO(AngleUtils::radian().ANGLE_90_IN_RADIANS, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	const JumpInfo JumpHelper::JUMP_FORWARD_INFO(AngleUtils::radian().ANGLE_45_IN_RADIANS, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);

	void LedgeDetector::start()
	{
   		_up = 0;
		_upFailed = false;
		_down = 0;
		_downFailed = false;
		_front = 0;
		_frontFailed = false;
		_height = 0.0f;

		OBB& body = *static_cast<OBB*>(getOwner().raiseMessage(Message(MessageID::GET_SHAPE)));
		_body.setOBB(body);
		_max  = _body.getTop() + _body.getHeight();
		_side = getOrientation(getOwner());
	}

	void LedgeDetector::handleUp(const Contact& contact)
	{
		if(_upFailed)
			return;
		const OBB& sensor = contact.getSource().getGlobalShape();
		const OBB& platform = contact.getTarget().getGlobalShape();
		if(platform.getAxisX().getY() == 0.0f && 
		   equals(_body.getTop(), platform.getCenterY()))
		{
			_up = const_cast<Fixture*>(&contact.getTarget());
		}
		else
		{
			OBB test = OBBAABB(_body.getCenter() + Vector(0.0f, _body.getHeight()), _body.getRadius()); 
			
			if(intersectsExclusive(test, platform))
			{
				_upFailed = true;
				_up = 0;
			}
		}
	}

	void LedgeDetector::handleDown(const Contact& contact)
	{
		if(_downFailed)
			return;
		const OBB& sensor = contact.getSource().getGlobalShape();
		const OBB& platform = contact.getTarget().getGlobalShape();
		if(platform.getAxisX().getY() == 0.0f && 
		   equals(_body.getBottom(), platform.getCenterY()))
		{
			_down = const_cast<Fixture*>(&contact.getTarget());;
		}
		else
		{
			OBB test = OBBAABB(_body.getCenter() - Vector(0.0f, _body.getHeight()), _body.getRadius()); 
			Vector correction = Vector::Zero;
			if(intersectsExclusive(test, platform))
			{
				_downFailed = true;
				_down = 0;
			}
		}
	}

	void LedgeDetector::handleFrontCheckY(float y)
	{
		if(y > _body.getTop())
		{
			_max = std::fminf(_max, y);
		}
		// Blocks from below
		else if(y > _body.getBottom())
		{
			_front = 0;
			_frontFailed = true;
		}
	}

	void LedgeDetector::handleFront(const Contact& contact)
	{
		if(_frontFailed)
			return;
		const OBB& platform = contact.getTarget().getGlobalShape();

		Side::Enum opposite = Side::getOpposite(_side);
		if(platform.getAxisX().getY() == 0.0f &&
			equals(_body.getSide(_side), platform.getSide(opposite)) &&
			_body.getTop() >= platform.getTop() &&
			_body.getBottom() <= platform.getTop())
		{
			float height = platform.getTop() - _body.getBottom();
			if (height > _height)
			{
				_height = height;
				_front = const_cast<Fixture*>(&contact.getTarget());
			}
				
		}
		else
		{
			float x = _body.getSide(_side);
			if (x >= platform.getLeft() && x <= platform.getRight())
			{
				handleFrontCheckY(platform.getTopSegment(x).getY(x));
				handleFrontCheckY(platform.getBottomSegment(x).getY(x));
			}
			
		}
	}

	void LedgeDetector::handle(const Contact& contact)
	{
		handleUp(contact);
		handleDown(contact);
		handleFront(contact);
	}

	void LedgeDetector::end()
	{
		if(_up)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_UP, _up));
		if(_down)
			getOwner().raiseMessage(Message(MessageID::SENSOR_DESCEND, _down));

		// Blocks from above
		if(_body.getTop() + _height > _max)
		{
			_front = 0;
			_frontFailed = true;
		}
		if(_front)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_FRONT, _front));
	}

	bool HandleMessageHelper::handleStandWalkMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if(message.getID() == MessageID::SENSOR_DESCEND)
		{
			if(_isDescending)
				_controller.changeState(DESCEND_STATE, message.getParam());
		}
		else if(message.getID() == MessageID::ACTION_ACTIVATE)
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
				_controller.changeState(DRAG_STAND_STATE, &draggable);
			}
			else
			{
				_isActivating = true;
			}
		}
		else if(message.getID() == MessageID::ACTION_TAKEDOWN)
		{
			_isTakingDown = true;
		}
		else if(message.getID() == MessageID::ACTION_AIM)
		{
			_controller.changeState(AIM_STATE);
		}
		else if (message.getID() == MessageID::SENSOR_SENSE)
		{
			SensorParams& params = getSensorParams(message.getParam());
			if(_isActivating && params.getSensorId() == ACTIVATE_SENSOR_ID && params.getContact().getTarget().getCategory() == CollisionCategory::BUTTON)
			{
				Hash entityId = params.getContact().getTarget().getEntityId();
				_controller.getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::ACTIVATE));
				params.setHandled(true);
			}
			else if(_isTakingDown && params.getSensorId() == TAKEDOWN_SENSOR_ID)
			{
				void* isCovered = _controller.raiseMessage(Message(MessageID::IS_COVERED));
				if (!isCovered || !*static_cast<bool*>(isCovered))
				{
					Hash entityId = params.getContact().getTarget().getEntityId();
					_controller.changeState(TAKEDOWN_STATE, &entityId);
				}
			}
		}
		else 
		{
			return false;
		}
		return true;
	}

	bool HandleMessageHelper::handleFallJumpMessage(Message& message)
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if(collision.getY() < 0.0f && AngleUtils::radian().isModerate(collision.getRightNormal().getAngle()))
			{
				_controller.changeState(JUMP_END_STATE);
				return true;
			}
			
		}
		return false;
	}

	bool HandleMessageHelper::handleDragMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTION_ACTIVATE)
		{
			_controller.changeState(STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::POST_LOAD)
		{
			_controller.changeState(STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::TEMPORAL_ECHOS_MERGED)
		{
			_controller.changeState(STAND_STATE);
			return true;
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			Hash draggableId = getActionController(&_controller).getDraggableId();
			const Vector& velocity = *static_cast<const Vector*>(_controller.getEntity().getManager().sendMessageToEntity(draggableId, Message(MessageID::GET_VELOCITY)));
			if (velocity.getY() != 0.0f)
			{
				_controller.changeState(STAND_STATE);
				return true;
			}
			else if (_controller.getCurrentStateID() != DRAG_STAND_STATE)
			{
				if (!_controller.getFrameFlag1())
				{
					_controller.changeState(DRAG_STAND_STATE);
					return true;
				}
				else
				{
					Vector movement = Vector(getActionController(&_controller).MAX_WALK_FORCE_PER_SECOND / 2.0f, 0.0f);
					if (_controller.getCurrentStateID() == DRAG_BACKWARDS_STATE)
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

	void HandleMessageHelper::handleDragEnter()
	{
		Hash draggableId = getActionController(&_controller).getDraggableId();
		if (_controller.getEntity().getManager().getEntity(draggableId)->get(ActionController::TYPE))
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DRAG_ANIMATION, false, 0.0f, 1)));
		}
		else
		{
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AIM_DOWN_ANIMATION, false, 0.0f, 1)));
			_controller.raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AIM_UP_ANIMATION, false, 0.0f, 2, 0.5f)));
		}
	}

	void HandleMessageHelper::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::DIE)
		{
			if (_controller.getCurrentStateID() != DIE_STATE)
				_controller.changeState(DIE_STATE);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = _isActivating = _isTakingDown = false;
		}
		else if (message.getID() == MessageID::LOST_GROUND)
		{
			if (_controller.getCurrentStateID() != FALL_STATE && _controller.getCurrentStateID() != JUMP_STATE)
			{
				if (_controller.getCurrentStateID() == DESCEND_STATE)
				{
					bool bodyEnabled = true;
					_controller.raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
				}
				_controller.changeState(FALL_STATE);
			}
		}
	}

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	ActionController::ActionController(float maxWalkForcePerSecond, Hash initialStateId) :
		StateMachineComponent(getStates(), "ACT", initialStateId), _ledgeDetector(LEDGE_SENSOR_ID, *this), _handleMessageHelper(*this), MAX_WALK_FORCE_PER_SECOND(maxWalkForcePerSecond), _isInvestigated(false) {}

	void ActionController::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::GET_DRAGGABLE)
		{
			if (getCurrentStateID() != DRAG_STAND_STATE && getCurrentStateID() != DRAG_FORWARD_STATE && getCurrentStateID() != DRAG_BACKWARDS_STATE)
				_draggableId = Hash::INVALID;
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
		states[STAND_STATE] = new Stand();
		states[FALL_STATE] = new Fall();
		states[WALK_STATE] = new Walk();
		states[TURN_STATE] = new Turn();
		states[JUMP_STATE] = new Jump();
		states[JUMP_END_STATE] = new JumpEnd();
		states[HANG_STATE] = new Hang();
		states[CLIMB_STATE] = new Climb();
		states[DESCEND_STATE] = new Descend();
		states[TAKEDOWN_STATE] = new Takedown();
		states[DIE_STATE] = new Die();
		states[DEAD_STATE] = new Dead();
		states[AIM_STATE] = new Aim();
		states[FIRE_STATE] = new Fire();
		states[DRAG_STAND_STATE] = new DragStand();
		states[DRAG_FORWARD_STATE] = new DragForward();
		states[DRAG_BACKWARDS_STATE] = new DragBackwards();
		states[INVESTIGATE_STATE] = new Investigate();
		return states;
	}

	Hash ActionController::getInitialState() const { return FALL_STATE; }

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/

	namespace ActionControllerStates
	{
		void Stand::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(STAND_ANIMATION)));
		}

		void Stand::handleMessage(Message& message)
		{
			if(getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(TURN_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_START)
			{
				JumpHelper helper(JumpType::UP);
				_stateMachine->changeState(JUMP_STATE, &helper);
			}
		}

		void Fall::enter(void* param)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(FALL_ANIMATION)));
		}

		void Fall::handleMessage(Message& message)
		{
			// TempFlag 1 - want to hang
			if(getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_UP_START)
			{
				// Need to be in start jump start, because can occur on fall from ledge
				if(_stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
				{
					JumpHelper helper(JumpType::FORWARD);
					_stateMachine->changeState(JUMP_STATE, &helper);
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
			if(getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_UP_START)
			{
				JumpHelper helper(JumpType::FORWARD);
				_stateMachine->changeState(JUMP_STATE, &helper);
			}
			// TempFlag 1 - still walking
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_INVESTIGATE)
			{
				_stateMachine->changeState(INVESTIGATE_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				if (!_stateMachine->getStateFlag())
				{
					_stateMachine->setStateFlag(true);
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION)));
				}
				if (!_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(STAND_STATE);
				}
				else
				{
					// We need to apply this every update because the ground has infinite restitution. 
					float x = ActionController::WALK_ACC_PER_SECOND * _stateMachine->getTimer().getElapsedTime();
					if (x > getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND)
						x = getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND;
					Vector force = Vector(x, 0.0f);
					_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
				}
			}
			else if (message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if (sameSign(side, collision.getX()) && !AngleUtils::radian().isSteep(collision.getAngle()))
				{
					_stateMachine->changeState(STAND_STATE);
				}
			}
		}

		void Turn::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(TURN_ANIMATION)));
		}

		void Turn::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
				_stateMachine->changeState(WALK_STATE);
			}
		}

		void Jump::enter(void* param)
		{
			const JumpHelper& jumpHelper = *static_cast<JumpHelper*>(param);
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
			if(getActionController(_stateMachine).getHandleMessageHelper().handleFallJumpMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::SENSOR_HANG_UP)
			{
				if(_stateMachine->getFrameFlag1())
					_stateMachine->changeState(HANG_STATE, message.getParam());
			}
			else if(message.getID() == MessageID::SENSOR_HANG_FRONT)
			{
				_stateMachine->changeState(CLIMB_STATE, message.getParam());
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
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(STAND_ANIMATION)));
		}

		void JumpEnd::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				// BRODER
				if (_stateMachine->getTimer().getElapsedTime() > 0.2f)
					_stateMachine->changeState(STAND_STATE);
			}
		}

		void Hang::enter(void* param)
		{
			bool bodyEnabled = false; 
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
			if(param)
				_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(HANG_ANIMATION)));
		}

		void Hang::exit(void* param)
		{
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
		}

		void Hang::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ACTION_DOWN)
			{	
				_stateMachine->changeState(FALL_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{	
				_stateMachine->changeState(CLIMB_STATE);
			}
		}

		void Climb::enter(void* param)
		{
			bool bodyEnabled = false; 
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
			const SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			getActionController(_stateMachine).setHangDescendOriginalTranslation(root.getTranslation());
			const OBBAABBWrapper shape(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
			float interpolation = 0.0f;
			Fixture* ground = 0;
			if(!param)
			{
				getActionController(_stateMachine).setHangDescendMovement(Vector(0.0f, shape.getHeight()));
				interpolation = 0.0f;
				ground = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
			}
			else
			{
				_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));
				Vector movement = Vector(shape.getWidth() *  getOrientation(*_stateMachine), static_cast<Fixture*>(param)->getGlobalShape().getTop() - shape.getBottom());
				getActionController(_stateMachine).setHangDescendMovement(movement);
				interpolation = 1.0f - (movement.getY() / shape.getHeight());
				ground = static_cast<Fixture*>(param);
			}
			
			Vector groundDelta = getPosition(*_stateMachine) - ground->getGlobalShape().getCenter();
			getActionController(_stateMachine).setHangDescendGroundDelta(groundDelta);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DESCEND_ANIMATION, true, interpolation)));
		}

		void Climb::exit(void* param)
		{
			SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			root.setTranslation(getActionController(_stateMachine).getHangDescendOriginalTranslation());
			bool bodyEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
		}

		void Climb::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
				float normalizedTime;
				_stateMachine->raiseMessage(Message(MessageID::GET_ANIMATION_NORMALIZED_TIME, &normalizedTime));
				const Vector& movement = getActionController(_stateMachine).getHangDescendMovement();
				Vector translation = normalizedTime * movement;
				Fixture* ground = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
				Vector startPosition = ground->getGlobalShape().getCenter() + getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition + translation;
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				
				root.setTranslation(getActionController(_stateMachine).getHangDescendOriginalTranslation() + Vector(0.0f, movement.getY()) - Vector(0.0f, translation.getY()));
			}
			else if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition + getActionController(_stateMachine).getHangDescendMovement();
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void Descend::enter(void* param)
		{
			bool bodyEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &bodyEnabled));
			_stateMachine->raiseMessage(Message(MessageID::SET_GROUND, param));

			Vector groundDelta = getPosition(*_stateMachine) - static_cast<Fixture*>(param)->getGlobalShape().getCenter();
			getActionController(_stateMachine).setHangDescendGroundDelta(groundDelta);

			const SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			getActionController(_stateMachine).setHangDescendOriginalTranslation(root.getTranslation());
			
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DESCEND_ANIMATION)));
		}

		void Descend::exit(void* param)
		{
			SceneNode& root = *static_cast<SceneNode*>(_stateMachine->raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			root.setTranslation(getActionController(_stateMachine).getHangDescendOriginalTranslation());
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
					getActionController(_stateMachine).getHangDescendGroundDelta();
				Vector position = startPosition - Vector(0.0f, translation);
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				root.setTranslation(Vector(root.getTranslation().getX(), getActionController(_stateMachine).getHangDescendOriginalTranslation().getY() + translation));
			}
			else if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				const OBBAABBWrapper size(static_cast<OBB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE))));
				Vector position = getPosition(*_stateMachine);
				Vector startPosition = static_cast<Fixture*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)))->getGlobalShape().getCenter() +
					getActionController(_stateMachine).getHangDescendGroundDelta();
				position.setY(startPosition.getY() - size.getHeight());
				_stateMachine->raiseMessage(Message(MessageID::SET_POSITION, &position));
				_stateMachine->changeState(HANG_STATE);
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
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(TAKEDOWN_ANIMATION)));
		}

		void Takedown::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void Die::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DIE_ANIMATION)));
			int category = CollisionCategory::DEAD | CollisionCategory::DRAGGABLE;
			_stateMachine->raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &category));
			DynamicBody& body = *static_cast<DynamicBody*>(_stateMachine->getEntity().get(DynamicBody::TYPE));
			body.getFixture().getLocalShape().setRadius(Vector(body.getFixture().getLocalShape().getRadiusY(), body.getFixture().getLocalShape().getRadiusY()));
			_stateMachine->getEntity().getManager().getGameState().getGrid().update(&body.getFixture());
			TemporalPeriod* period = static_cast<TemporalPeriod*>(_stateMachine->getEntity().get(TemporalPeriod::TYPE));
			if (period)
				period->setCreateFutureSelf(true);
			
		}

		void Die::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(DEAD_STATE);
			}
		}

		void Dead::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DEAD_ANIMATION)));
			
		}

		void Aim::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(STAND_ANIMATION)));
		}

		void Aim::handleMessage(Message& message)
		{
			if (getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if(message.getID() == MessageID::ACTION_FIRE)
			{
				// Temp flag 1 - Firing
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::ACTION_HOLSTER)
			{
				_stateMachine->changeState(STAND_STATE);
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
					_stateMachine->changeState(FIRE_STATE, &weight);
				}
				else
				{
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AIM_DOWN_ANIMATION, false, 0.0f, 1)));
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(AIM_UP_ANIMATION, false, 0.0f, 2, weight)));
				}
			}
		}

		static const Hash PLAYER("ENT_PLAYER");

		void Fire::enter(void* param)
		{
			float weight = getFloatParam(param);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(FIRE_DOWN_ANIMATION, false, 0.0f, 1)));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(FIRE_UP_ANIMATION, false, 0.0f, 2, weight)));
			_stateMachine->getEntity().getManager().sendMessageToEntity(Hash(PLAYER), Message(MessageID::DIE));
		}

		void Fire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void DragStand::enter(void* param)
		{
			if (param)
				getActionController(_stateMachine).setDraggableId(getHashParam(param));

			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(STAND_ANIMATION)));
			getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
		}

		void DragStand::handleMessage(Message& message)
		{
			if (getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{

			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(DRAG_FORWARD_STATE);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(DRAG_BACKWARDS_STATE);
			}
		}

		void DragForward::enter(void* param)
		{
			// Frame flag 1 - Still walking forward
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION)));
			getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragForward::handleMessage(Message& message)
		{
			if (getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->changeState(DRAG_BACKWARDS_STATE);
			}
		}

		void DragBackwards::enter(void* param)
		{
			// Frame flag 1 - Still walking backwards
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION, true)));
			getActionController(_stateMachine).getHandleMessageHelper().handleDragEnter();
			_stateMachine->setFrameFlag1(true);
		}

		void DragBackwards::handleMessage(Message& message)
		{
			if (getActionController(_stateMachine).getHandleMessageHelper().handleDragMessage(message))
			{
			}
			else if (message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->changeState(DRAG_FORWARD_STATE); 
			}
			else if (message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
		}

		void Investigate::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(INVESTIGATE_ANIMATION)));
		}

		void Investigate::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(STAND_STATE);
			}
		}
	}
}