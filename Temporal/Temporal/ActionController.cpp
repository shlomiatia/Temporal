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
	const Hash ActionController::TYPE = Hash("action-controller");
	float ActionController::WALK_ACC_PER_SECOND(2500.0f);
	float ActionController::MAX_WALK_FORCE_PER_SECOND(250.0f);
	float ActionController::JUMP_FORCE_PER_SECOND(400.0f);
	float ActionController::FALL_ALLOW_JUMP_TIME(0.15f);
	float ActionController::JUMP_STOP_MODIFIER(0.5f);
	float ActionController::MAX_WALK_JUMP_MODIFIER(1.15f);

	static const Hash DESCEND_SENSOR_ID = Hash("SNS_DESCEND");
	static const Hash HANG_SENSOR_ID = Hash("SNS_HANG");
	static const Hash ACTIVATE_SENSOR_ID = Hash("SNS_ACTIVATE");

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

	const JumpInfo JumpHelper::JUMP_UP_INFO(ANGLE_90_IN_RADIANS, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	const JumpInfo JumpHelper::JUMP_FORWARD_INFO(ANGLE_45_IN_RADIANS, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);

	void LedgeDetector::start()
	{
		_isFound = false;
		_isFailed = false;
	}

	void LedgeDetector::handle(const Contact& contact)
	{
		const YABP& actor = contact.getSource().getGlobalShape();
		const YABP& platform = contact.getTarget().getGlobalShape();
		if(platform.getHeight() == 0.0f && 
		   (equals(actor.getTop(), platform.getTop()) || equals(actor.getBottom() ,platform.getBottom())) &&
		   !_isFailed)
		{
			_isFound = true;
		}
		else
		{
			_isFailed = true;
			_isFound = false;
		}
	}

	void LedgeDetector::end(Component& component)
	{
		if(_isFound)
			component.raiseMessage(Message(_messageId));
	}

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	ActionController::ActionController() :
		StateMachineComponent(getStates(), "ACT"),
		_hangDetector(HANG_SENSOR_ID, MessageID::SENSOR_HANG),
		_descendDetector(DESCEND_SENSOR_ID, MessageID::SENSOR_DESCEND) {}

	void ActionController::handleMessage(Message& message)
	{
		_hangDetector.handleMessage(*this, message);
		_descendDetector.handleMessage(*this, message);
		StateMachineComponent::handleMessage(message);
	}

	StateCollection ActionController::getStates() const
	{
		using namespace ActionControllerStates;
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

	namespace ActionControllerStates
	{
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
			else if(message.getID() == MessageID::SENSOR_DESCEND)
			{
				if(_stateMachine->getTempFlag1())
					_stateMachine->changeState(DESCEND_STATE);
			}
			// TempFlag2 - is activating
			else if(message.getID() == MessageID::ACTION_ACTIVATE)
			{
				_stateMachine->setTempFlag2(true);
			}
			else if (_stateMachine->getTempFlag2() && message.getID() == MessageID::SENSOR_SENSE)
			{
				const SensorParams& params = getSensorParams(message.getParam());
				if(params.getSensorId() == ACTIVATE_SENSOR_ID)
				{
					Hash entityId = params.getContact().getTarget().getEntityId();
					_stateMachine->getEntity().getManager().sendMessageToEntity(entityId, Message(MessageID::ACTIVATE));
				}
			}
		}

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
				// Need to be in start jump start, because can occur on fall from ledge
				if(_stateMachine->getTimer().getElapsedTime() <= ActionController::FALL_ALLOW_JUMP_TIME)
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
					float x = ActionController::WALK_ACC_PER_SECOND * _stateMachine->getTimer().getElapsedTime();
					if(x > ActionController::MAX_WALK_FORCE_PER_SECOND)
						x = ActionController::MAX_WALK_FORCE_PER_SECOND; 
					Vector force = Vector(x, 0.0f);
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
			Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
			float force = ActionController::JUMP_FORCE_PER_SECOND + velocity.getLength();
			float max = ActionController::JUMP_FORCE_PER_SECOND * ActionController::MAX_WALK_JUMP_MODIFIER;
			if(force > max)
				force = max;
			float jumpForceX = force * cos(angle);
			float jumpForceY = force * sin(angle);
			Vector jumpVector = Vector(jumpForceX, jumpForceY);
			_stateMachine->raiseMessage(Message(MessageID::SET_TIME_BASED_IMPULSE, &jumpVector));
			Hash animation = jumpHelper.getInfo().getJumpAnimation();
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &animation));

			// Pressed on transition
			_stateMachine->setTempFlag1(true);
		}

		void Jump::handleMessage(Message& message) const
		{
			
			// TempFlag 1 - Want to hang
			if(message.getID() == MessageID::ACTION_UP)
			{
				_stateMachine->setTempFlag1(true);
			}
			else if (message.getID() == MessageID::SENSOR_HANG)
			{
				if(_stateMachine->getTempFlag1())
					_stateMachine->changeState(HANG_STATE);
			}
			else if(message.getID() == MessageID::BODY_COLLISION)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if(collision.getY() < 0.0f)
					_stateMachine->changeState(JUMP_END_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				// Permanent flag - stopped jumping
				if(!_stateMachine->getPermanentFlag() && !_stateMachine->getTempFlag1())
				{
					Vector& velocity = *static_cast<Vector*>(_stateMachine->raiseMessage(Message(MessageID::GET_VELOCITY)));
					if(velocity.getY() > 0.0f)
					{
						velocity.setY(velocity.getY() * ActionController::JUMP_STOP_MODIFIER);
						_stateMachine->setPermanentFlag(true);
					}
				}
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
}