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
	static Hash AIM_DOWN_ANIMATION = Hash("POP_ANM_AIM_DOWN");
	static Hash AIM_UP_ANIMATION = Hash("POP_ANM_AIM_UP");
	static Hash FIRE_DOWN_ANIMATION = Hash("POP_ANM_FIRE_DOWN");
	static Hash FIRE_UP_ANIMATION = Hash("POP_ANM_FIRE_UP");

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
	static const Hash AIM_STATE = Hash("ACT_STT_AIM");
	static const Hash FIRE_STATE = Hash("ACT_STT_FIRE");
	static const Hash DRAG_STATE = Hash("ACT_STT_DRAG");

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

	bool intersectsExclusive(const OBB& o1, const OBB& o2)
	{
		Vector correction = Vector::Zero;
		return(intersects(o1, o2, &correction) && (!equals(correction.getX(), 0.0f) || !equals(correction.getY(), 0.0f)));
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
		float topSide = platform.getTop();
		if(platform.getAxisX().getY() == 0.0f &&
			equals(_body.getSide(_side), platform.getSide(opposite)) &&
			_body.getTop() >= topSide &&
			_body.getBottom() <= topSide)
		{
			float height = topSide - _body.getBottom();
			if (height > _height)
			{
				_height = height;
				_front = const_cast<Fixture*>(&contact.getTarget());
			}
				
		}
		else
		{
			handleFrontCheckY(platform.getTop());
			handleFrontCheckY(platform.getBottom());
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
			_isActivating = true;
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
			if(collision.getY() < 0.0f)
			{
				
				_controller.changeState(JUMP_END_STATE);
				return true;
			}
			
		}
		return false;
	}

	bool HandleMessageHelper::handleWalkDragMessage(Message& message)
	{
		if (message.getID() == MessageID::ACTION_UP_START)
		{
			JumpHelper helper(JumpType::FORWARD);
			_controller.changeState(JUMP_STATE, &helper);
			return true;
		}
		// TempFlag 1 - still walking
		else if (message.getID() == MessageID::ACTION_FORWARD)
		{
			_controller.setFrameFlag1(true);
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			if (!_controller.getFrameFlag1())
			{
				_controller.changeState(STAND_STATE);
				return true;
			}
			else
			{
				// We need to apply this every update because the ground has infinite restitution. 
				float x = ActionController::WALK_ACC_PER_SECOND * _controller.getTimer().getElapsedTime();
				if (x > getActionController(&_controller).MAX_WALK_FORCE_PER_SECOND)
					x = getActionController(&_controller).MAX_WALK_FORCE_PER_SECOND;
				Vector force = Vector(x, 0.0f);
				_controller.raiseMessage(Message(MessageID::SET_IMPULSE, &force));
			}
		}
		return false;
	}

	void HandleMessageHelper::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::DIE && _controller.getCurrentStateID() != DIE_STATE)
		{
			_controller.changeState(DIE_STATE);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = _isActivating = _isTakingDown = false;
			void* ground = _controller.raiseMessage(Message(MessageID::GET_GROUND));
		}
		else if (message.getID() == MessageID::LOST_GROUND)
		{
			bool bodyEnabled = getBoolParam(_controller.raiseMessage(Message(MessageID::IS_BODY_ENABLED)));
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
	ActionController::ActionController(float maxWalkForcePerSecond) :
		StateMachineComponent(getStates(), "ACT"), _ledgeDetector(LEDGE_SENSOR_ID, *this), _handleMessageHelper(*this), MAX_WALK_FORCE_PER_SECOND(maxWalkForcePerSecond) {}

	void ActionController::handleMessage(Message& message)
	{
		
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
		states[AIM_STATE] = new Aim();
		states[FIRE_STATE] = new Fire();
		states[DRAG_STATE] = new Drag();
		return states;
	}

	Hash ActionController::getInitialState() const { return STAND_STATE; }

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
			
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION)));
		}

		void Walk::handleMessage(Message& message)
		{
			if(getActionController(_stateMachine).getHandleMessageHelper().handleStandWalkMessage(message))
			{
			}
			else if (getActionController(_stateMachine).getHandleMessageHelper().handleWalkDragMessage(message))
			{
			}
			else if (message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if (sameSign(side, collision.getX()) && !AngleUtils::radian().isSteep(collision.getAngle()))
				{
					int sourceCollisionGroup = getIntParam(_stateMachine->raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
					const Vector& position = getPosition(*_stateMachine);
					RayCastResult result;
					if (_stateMachine->getEntity().getManager().getGameState().getGrid().cast(position, Vector(side, 0.0f), result, CollisionCategory::OBSTACLE | CollisionCategory::DRAGGABLE, sourceCollisionGroup))
					{
						Hash draggable = result.getFixture().getEntityId();
						_stateMachine->changeState(DRAG_STATE, &draggable);
					}
					else
					{
						_stateMachine->changeState(STAND_STATE);
					}
					
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
				if(!_stateMachine->getStateFlag() && !_stateMachine->getFrameFlag1())
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
			int category = CollisionCategory::NONE;
			_stateMachine->raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &category));
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
				const Vector& vector = getVectorParam(message.getParam());
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

		void Drag::enter(void* param)
		{
			getActionController(_stateMachine).setDraggableId(getHashParam(param));
			// TempFlag 1 - still walking
			_stateMachine->setFrameFlag1(true);

			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION)));
		}

		void Drag::handleMessage(Message& message)
		{
			if (getActionController(_stateMachine).getHandleMessageHelper().handleWalkDragMessage(message))
			{

			}
			else if (message.getID() == MessageID::SET_IMPULSE)
			{
				Hash draggableId = getActionController(_stateMachine).getDraggableId();
				Vector movement = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				movement.setX(movement.getX() * side);
				_stateMachine->getEntity().getManager().sendMessageToEntity(draggableId, Message(MessageID::SET_IMPULSE, &movement));
			}
			else if (message.getID() == MessageID::POST_LOAD)
			{
				_stateMachine->changeState(WALK_STATE);
			}
			else if (message.getID() == MessageID::TEMPORAL_PERIOD_CHANGED)
			{
				_stateMachine->changeState(WALK_STATE);
			}
			else if (message.getID() == MessageID::TEMPORAL_ECHOS_MERGED)
			{
				_stateMachine->changeState(WALK_STATE);
			}
		}
	}
}