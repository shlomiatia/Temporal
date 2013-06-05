#include "ActionController.h"
#include "MovementUtils.h"
#include "MessageUtils.h"
#include "Serialization.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "DynamicBody.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Constants
	 *********************************************************************************************/
	static const Hash DESCEND_SENSOR_ID = Hash("SNS_DESCEND");
	static const Hash HANG_SENSOR_ID = Hash("SNS_HANG");

	static Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
	static Hash TURN_ANIMATION = Hash("POP_ANM_TURN");
	static Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
	static Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP_UP");
	static Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
	static Hash CLIMB_ANIMATION = Hash("POP_ANM_CLIMB");
	static Hash DESCEND_ANIMATION = Hash("POP_ANM_DESCEND");
	static Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP_FORWARD");
	static Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_JUMP_FORWARD_END");
	static Hash WALK_ANIMATION = Hash("POP_ANM_WALK");

	static const Hash STAND_STATE = Hash("ACT_STT_STAND");
	static const Hash FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash WALK_STATE = Hash("ACT_STT_WALK");
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

	bool isLedgeDetectionMessage(Message& message, Hash sensorID)
	{
		if(message.getID() == MessageID::LEDGE_DETECTED)
		{
			const Hash& id = getHashParam(message.getParam());
			if(id == sensorID)
				return true;
		}
		return false;
	}

	const JumpInfo JumpHelper::JUMP_UP_INFO(ANGLE_90_IN_RADIANS, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	const JumpInfo JumpHelper::JUMP_FORWARD_INFO(ANGLE_45_IN_RADIANS, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	StateCollection ActionController::getStates() const
	{
		StateCollection states;
		states[STAND_STATE] = new Stand();
		states[FALL_STATE] = new Fall();
		states[WALK_STATE] = new Walk();
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
	void Stand::enter() const
	{
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &STAND_ANIMATION));
	}

	void Stand::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine->changeState(WALK_STATE);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(TURN_STATE);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{
			getActionController(_stateMachine).getJumpHelper().setType(JumpType::UP);
			_stateMachine->changeState(JUMP_STATE);
		}
		// TempFlag1 - Is descending
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if(_stateMachine->getTempFlag1() && isLedgeDetectionMessage(message, DESCEND_SENSOR_ID))
		{
			_stateMachine->changeState(DESCEND_STATE);
		}
	}

	const float Fall::ALLOW_JUMP_TIME = 0.075f;

	void Fall::enter() const
	{
		// Not setting force because we want to continue the momentum
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &FALL_ANIMATION));
	}

	void Fall::handleMessage(Message& message) const
	{
		// TempFlag 1 - want to hang
		if(message.getID() == MessageID::ACTION_UP)
		{
			if(_stateMachine->getTimer().getElapsedTime() <= ALLOW_JUMP_TIME)
			{
				getActionController(_stateMachine).getJumpHelper().setType(JumpType::FORWARD);
				_stateMachine->changeState(JUMP_STATE);
			}
			else
			{
				_stateMachine->setTempFlag1(true);
			}
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if(collision.getY() < 0.0f)
				_stateMachine->changeState(STAND_STATE);
		}
	}

	void Walk::enter() const
	{
		// TempFlag 1 - still walking
		_stateMachine->setTempFlag1(true);
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &WALK_ANIMATION));
	}

	void Walk::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			getActionController(_stateMachine).getJumpHelper().setType(JumpType::FORWARD);
			_stateMachine->changeState(JUMP_STATE);
		}
		// TempFlag 1 - still walking
		// TempFlag 2 - no floor
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if(message.getID() == MessageID::UPDATE)
		{			
			void* ground = _stateMachine->raiseMessage(Message(MessageID::GET_GROUND));
			if(!ground)
			{
				_stateMachine->changeState(FALL_STATE);
			}
			else if(!_stateMachine->getTempFlag1())
			{
				_stateMachine->changeState(STAND_STATE);
			}
			else
			{
				// We need to apply this every update because the ground has infinite restitution. 
				Vector force = Vector(WALK_FORCE_PER_SECOND, 0.0f);
				_stateMachine->raiseMessage(Message(MessageID::SET_TIME_BASED_IMPULSE, &force));
			}
		}
	}

	void Turn::enter() const
	{
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &TURN_ANIMATION));
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
		float jumpForceX = JUMP_FORCE_PER_SECOND * cos(angle);
		float jumpForceY = JUMP_FORCE_PER_SECOND * sin(angle);
		Vector jumpVector = Vector(jumpForceX, jumpForceY);
		_stateMachine->raiseMessage(Message(MessageID::SET_TIME_BASED_IMPULSE, &jumpVector));
		Hash animation = jumpHelper.getInfo().getJumpAnimation();
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &animation));
	}

	void Jump::handleMessage(Message& message) const
	{
		// TempFlag 1 - Want to hang
		if(message.getID() == MessageID::ACTION_UP)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if (_stateMachine->getTempFlag1() && isLedgeDetectionMessage(message, HANG_SENSOR_ID))
		{
			_stateMachine->changeState(HANG_STATE);
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if(collision.getY() < 0.0f)
				_stateMachine->changeState(JUMP_END_STATE);
		}
	}

	void JumpEnd::enter() const
	{
		Hash animation = getActionController(_stateMachine).getJumpHelper().getInfo().getEndAnimation();
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &animation));
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
		const YABP& personBounds = *static_cast<YABP*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float personCenterX = personBounds.getCenterX();
		Vector drawPosition(personCenterX, personBounds.getTop());
		_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
		bool gravityEnabled = false;
		_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &HANG_ANIMATION));
	}

	void Hang::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			Vector zero = Vector::Zero;
			_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &zero));
			bool gravityEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
			_stateMachine->changeState(FALL_STATE);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(CLIMB_STATE);
		}
	}

	void Climb::enter() const
	{
		const YABP& shape = *static_cast<YABP*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float climbForceY = shape.getHeight();
		Vector climbForce(0.0f, climbForceY);

		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &CLIMB_ANIMATION));
		_stateMachine->raiseMessage(Message(MessageID::SET_ABSOLUTE_IMPULSE, &climbForce));
	}

	void Climb::exit() const
	{
		_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, const_cast<Vector*>(&Vector::Zero)));
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
		const YABP& personBounds = *static_cast<YABP*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float personBottom = personBounds.getBottom();
		float personCenterX = personBounds.getCenterX();
		Vector drawPosition(personCenterX, personBottom);
		_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
		bool gravityEnabled = false;
		_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		const YABP& size = *static_cast<YABP*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float forceX = 0.0f;
		float forceY = -(size.getHeight());

		_stateMachine->raiseMessage(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(forceX, forceY)));
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &DESCEND_ANIMATION));
	}

	void Descend::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(HANG_STATE);
		}
	}

}