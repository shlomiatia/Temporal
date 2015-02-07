#include "ActionController.h"
#include "MovementUtils.h"
#include "MessageUtils.h"
#include "Serialization.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "DynamicBody.h"
#include "ShapeOperations.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Constants
	 *********************************************************************************************/
	const Hash ActionController::TYPE = Hash("action-controller");
	float ActionController::WALK_ACC_PER_SECOND(1250.0f);
	float ActionController::JUMP_FORCE_PER_SECOND(400.0f);
	float ActionController::FALL_ALLOW_JUMP_TIME(0.15f);
	float ActionController::JUMP_STOP_MODIFIER(0.5f);
	float ActionController::MAX_WALK_JUMP_MODIFIER(1.15f);

	static const Hash LEDGE_SENSOR_ID = Hash("SNS_LEDGE");
	static const Hash ACTIVATE_SENSOR_ID = Hash("SNS_ACTIVATE");

	static Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
	static Hash TURN_ANIMATION = Hash("POP_ANM_BASE");
	static Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
	static Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP");
	static Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
	static Hash DESCEND_ANIMATION = Hash("POP_ANM_DESCEND");
	static Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP");
	static Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_BASE");
	static Hash WALK_ANIMATION = Hash("POP_ANM_WALK");

	static const Hash STAND_STATE = Hash("ACT_STT_STAND");
	static const Hash FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash WALK_STATE = Hash("ACT_STT_WALK");
	static const Hash SLIDE_STATE = Hash("ACT_STT_SLIDE");
	static const Hash TURN_STATE = Hash("ACT_STT_TURN");
	static const Hash JUMP_STATE = Hash("ACT_STT_JUMP");
	static const Hash JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
	static const Hash HANG_STATE = Hash("ACT_STT_HANG");
	static const Hash CLIMB_STATE = Hash("ACT_STT_CLIMB");
	static const Hash DESCEND_STATE = Hash("ACT_STT_DESCEND");

	/**********************************************************************************************
	 * Helpers
	 *********************************************************************************************/
	ActionController& getActionController(StateMachineComponent* stateMachine)
	{
		return *static_cast<ActionController*>(stateMachine);
	}

	const JumpInfo JumpHelper::JUMP_UP_INFO(ANGLE_90_IN_RADIANS, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	const JumpInfo JumpHelper::JUMP_FORWARD_INFO(ANGLE_45_IN_RADIANS, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);

	void LedgeDetector::start()
	{
   		_upFound = false;
		_upFailed = false;
		_downFound = false;
		_downFailed = false;
		_frontFound = false;
		_frontFailed = false;
		_height = 0.0f;

		_body = static_cast<const OBBAABBWrapper*>(getOwner().raiseMessage(Message(MessageID::GET_SHAPE)));
		_max  = _body->getTop() + _body->getHeight();
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
		   equals(_body->getTop(), platform.getCenterY()))
		{
			_upFound = true;
		}
		else
		{
			OBB test = OBBAABB(_body->getCenter() + Vector(0.0f, _body->getHeight()), _body->getRadius()); 
			
			if(intersectsExclusive(test, platform))
			{
				_upFailed = true;
				_upFound = false;
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
		   equals(_body->getBottom(), platform.getCenterY()))
		{
			_downFound = true;
		}
		else
		{
			OBB test = OBBAABB(_body->getCenter() - Vector(0.0f, _body->getHeight()), _body->getRadius()); 
			Vector correction = Vector::Zero;
			if(intersectsExclusive(test, platform))
			{
				_downFailed = true;
				_downFound = false;
			}
		}
	}

	void LedgeDetector::handleFrontCheckY(float y)
	{
		if(y > _body->getTop())
		{
			_max = std::min(_max, y);
		}
		// Blocks from below
		else if(y > _body->getBottom())
		{
			_frontFound = false;
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
			equals(_body->getSide(_side), platform.getSide(opposite)) &&
			_body->getTop() >= topSide &&
			_body->getBottom() <= topSide)
		{
			float height = topSide - _body->getBottom();
			if(height > _height)
				_height = height;
			_frontFound = true;
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
		if(_upFound)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_UP));
		if(_downFound)
			getOwner().raiseMessage(Message(MessageID::SENSOR_DESCEND));

		// Blocks from above
		if(_body->getTop() + _height > _max)
		{
			_frontFound = false;
			_frontFailed = true;
		}
		if(_frontFound)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_FRONT, &_height));
	}

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	ActionController::ActionController() :
		StateMachineComponent(getStates(), "ACT"), _ledgeDetector(LEDGE_SENSOR_ID, *this), _climbVector(Vector::Zero), MAX_WALK_FORCE_PER_SECOND(125.0f) {}

	void ActionController::handleMessage(Message& message)
	{
		_ledgeDetector.handleMessage(message);
		StateMachineComponent::handleMessage(message);
	}

	StateCollection ActionController::getStates() const
	{
		using namespace ActionControllerStates;
		StateCollection states;
		states[STAND_STATE] = new Stand();
		states[FALL_STATE] = new Fall();
		states[WALK_STATE] = new Walk();
		states[SLIDE_STATE] = new Slide();
		states[TURN_STATE] = new Turn();
		states[JUMP_STATE] = new Jump();
		states[JUMP_END_STATE] = new JumpEnd();
		states[HANG_STATE] = new Hang();
		states[CLIMB_STATE] = new Climb();
		states[DESCEND_STATE] = new Descend();
		return states;
	}

	Hash ActionController::getInitialState() const { return STAND_STATE; }

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/

	namespace ActionControllerStates
	{
		void Stand::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(STAND_ANIMATION)));
		}

		void Stand::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ACTION_FORWARD)
			{
				// Permanent flag 2 - front collision
				if(!_stateMachine->getStateMachineFlag())
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::ACTION_BACKWARD)
			{
				_stateMachine->setStateMachineFlag(false);
				_stateMachine->changeState(TURN_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_START)
			{
				getActionController(_stateMachine).getJumpHelper().setType(JumpType::UP);
				_stateMachine->changeState(JUMP_STATE);
			}
			// TempFlag1 - Is descending
			else if(message.getID() == MessageID::ACTION_DOWN)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::SENSOR_DESCEND)
			{
				if(_stateMachine->getFrameFlag1())
					_stateMachine->changeState(DESCEND_STATE);
			}
			// TempFlag2 - is activating
			else if(message.getID() == MessageID::ACTION_ACTIVATE)
			{
				_stateMachine->setFrameFlag2(true);
			}
			else if (_stateMachine->getFrameFlag2() && message.getID() == MessageID::SENSOR_SENSE)
			{
				const SensorParams& params = getSensorParams(message.getParam());
				if(params.getSensorId() == ACTIVATE_SENSOR_ID)
				{
					Hash entityId = params.getContact().getTarget().getEntityId();
					_stateMachine->getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::ACTIVATE));
				}
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				void* ground = _stateMachine->raiseMessage(Message(MessageID::GET_GROUND));
				if(!ground)
				{
					_stateMachine->changeState(FALL_STATE);
				}
			}
		}

		void Fall::enter() const
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(FALL_ANIMATION)));
		}

		void Fall::handleMessage(Message& message) const
		{
			// TempFlag 1 - want to hang
			if(message.getID() == MessageID::ACTION_UP_START)
			{
				// Need to be in start jump start, because can occur on fall from ledge
				if(_stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
				{
					getActionController(_stateMachine).getJumpHelper().setType(JumpType::FORWARD);
					_stateMachine->changeState(JUMP_STATE);
				}
				else
				{
					_stateMachine->setFrameFlag1(true);
				}
			}
			else if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if(collision.getY() < 0.0f)
				{
					const Segment& ground = *static_cast<Segment*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
					if(!isModerateAngle(ground.getNaturalVector().getAngle()))
						_stateMachine->changeState(SLIDE_STATE);
					else
						_stateMachine->changeState(STAND_STATE);
				}

			}
		}

		void Walk::enter() const
		{
			// TempFlag 1 - still walking
			_stateMachine->setFrameFlag1(true);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(WALK_ANIMATION)));
		}

		void Walk::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ACTION_UP_START)
			{
				getActionController(_stateMachine).getJumpHelper().setType(JumpType::FORWARD);
				_stateMachine->changeState(JUMP_STATE);
			}
			// TempFlag 1 - still walking
			// TempFlag 2 - no floor
			else if(message.getID() == MessageID::ACTION_FORWARD)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if(sameSign(side, collision.getX()) && !isSteepAngle(collision.getAngle()))
				{
					_stateMachine->setStateMachineFlag(true);
					_stateMachine->changeState(STAND_STATE);
				}
			}
			else if(message.getID() == MessageID::UPDATE)
			{			
				void* ground = _stateMachine->raiseMessage(Message(MessageID::GET_GROUND));
				if(!ground)
				{
					_stateMachine->changeState(FALL_STATE);
				}
				else if(!_stateMachine->getFrameFlag1())
				{
					_stateMachine->changeState(STAND_STATE);
				}
				else
				{
					// We need to apply this every update because the ground has infinite restitution. 
					float x = ActionController::WALK_ACC_PER_SECOND * _stateMachine->getTimer().getElapsedTime();
					if(x > getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND)
						x = getActionController(_stateMachine).MAX_WALK_FORCE_PER_SECOND; 
					Vector force = Vector(x, 0.0f);
					_stateMachine->raiseMessage(Message(MessageID::SET_IMPULSE, &force));
				}
			}
		}

		void Slide::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(JUMP_FORWARD_ANIMATION)));
		}

		void Slide::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::UPDATE)
			{
				const Segment* ground = static_cast<Segment*>(_stateMachine->raiseMessage(Message(MessageID::GET_GROUND)));
				if(!ground || isModerateAngle(ground->getNaturalVector().getAngle()))
					_stateMachine->changeState(STAND_STATE);
			}
		}

		void Turn::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(TURN_ANIMATION)));
		}

		void Turn::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void Jump::enter() const
		{
			const JumpHelper& jumpHelper = getActionController(_stateMachine).getJumpHelper();
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

		void Jump::handleMessage(Message& message) const
		{
			// TempFlag 1 - Want to hang
			if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if (message.getID() == MessageID::SENSOR_HANG_UP)
			{
				if(_stateMachine->getFrameFlag1())
					_stateMachine->changeState(HANG_STATE);
			}
			else if(message.getID() == MessageID::SENSOR_HANG_FRONT)
			{
				Vector climbVector(1.0f *  getOrientation(*_stateMachine), *(float*)message.getParam());
				getActionController(_stateMachine).setClimbVector(climbVector);
				_stateMachine->changeState(CLIMB_STATE);
			}

			else if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				Side::Enum side = getOrientation(*_stateMachine);
				if(sameSign(side, collision.getX()) && !isSteepAngle(collision.getAngle()) && _stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
				{
					// Permanenet flag 2 - front collision
					_stateMachine->setStateMachineFlag(true);
				}
				if(collision.getY() < 0.0f)
				{
					if(isModerateAngle(collision.getAngle()))
					{
						_stateMachine->changeState(SLIDE_STATE);
					}
					else
					{
						_stateMachine->changeState(STAND_STATE);
					}
				}
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				// Permanent flag 1 - stopped jumping
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

		

		void JumpEnd::enter() const
		{
			Hash animation = getActionController(_stateMachine).getJumpHelper().getInfo().getEndAnimation();
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(animation)));
		}

		void JumpEnd::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void Hang::enter() const
		{
			bool gravityEnabled = false; 
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(HANG_ANIMATION)));
		}

		void Hang::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ACTION_DOWN)
			{	
				getActionController(_stateMachine).setClimbVector(Vector::Zero);
				bool gravityEnabled = true;
				_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
				_stateMachine->changeState(FALL_STATE);
			}
			else if(message.getID() == MessageID::ACTION_UP_CONTINUE)
			{	
				_stateMachine->changeState(CLIMB_STATE);
			}
		}

		void Climb::enter() const
		{
			// Permanent flag - front collision
			_stateMachine->setStateMachineFlag(false);
			bool gravityEnabled = false; 
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
			const OBBAABBWrapper& shape = *static_cast<OBBAABBWrapper*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
			float bodyHeight = shape.getHeight();
			Vector climbForce = getActionController(_stateMachine).getClimbVector();
			if(climbForce == Vector::Zero)
				climbForce = Vector(0.0f, bodyHeight);
			
			getActionController(_stateMachine).setClimbVector(Vector::Zero);
			_stateMachine->raiseMessage(Message(MessageID::TRANSLATE_POSITION, &climbForce));
			float interpolation = 1.0f - (climbForce.getY() / bodyHeight);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DESCEND_ANIMATION, true, interpolation)));
		}

		void Climb::exit() const
		{
			bool gravityEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}

		void Climb::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(STAND_STATE);
			}
		}

		void Descend::enter() const
		{
			// Permanent flag - front collision
			_stateMachine->setStateMachineFlag(false);
			bool gravityEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(DESCEND_ANIMATION)));
		}

		void Descend::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				const OBBAABBWrapper& size = *static_cast<OBBAABBWrapper*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
				float forceX = 0.0f;
				float forceY = -(size.getHeight());
				Vector translation = Vector(forceX, forceY);
				_stateMachine->raiseMessage(Message(MessageID::TRANSLATE_POSITION, &translation));
				_stateMachine->changeState(HANG_STATE);
			}
		}
	}
}