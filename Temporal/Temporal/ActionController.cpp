#include "ActionController.h"
#include "MovementUtils.h"
#include "MessageUtils.h"
#include "Serialization.h"
#include "BaseUtils.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "DynamicBody.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Constants
	 *********************************************************************************************/
	static const Hash BACK_EDGE_SENSOR_ID = Hash("SNS_BACK_EDGE");
	static const Hash FRONT_EDGE_SENSOR_ID = Hash("SNS_FRONT_EDGE");
	static const Hash JUMP_SENSOR_ID = Hash("SNS_JUMP");
	static const Hash HANG_SENSOR_ID = Hash("SNS_HANG");

	static Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
	static Hash TURN_ANIMATION = Hash("POP_ANM_TURN");
	static Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
	static Hash JUMP_UP_START_ANIMATION = Hash("POP_ANM_JUMP_UP_START");
	static Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP_UP");
	static Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
	static Hash CLIMB_ANIMATION = Hash("POP_ANM_CLIMB");
	static Hash DESCEND_ANIMATION = Hash("POP_ANM_DESCEND");
	static Hash JUMP_FORWARD_START_ANIMATION = Hash("POP_ANM_JUMP_FORWARD_START");
	static Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP_FORWARD");
	static Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_JUMP_FORWARD_END");
	static Hash WALK_ANIMATION = Hash("POP_ANM_WALK");

	static const Hash STAND_STATE = Hash("ACT_STT_STAND");
	static const Hash FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash WALK_STATE = Hash("ACT_STT_WALK");
	static const Hash TURN_STATE = Hash("ACT_STT_TURN");
	static const Hash PREPARE_TO_JUMP_STATE = Hash("ACT_STT_PREPARE_TO_JUMP");
	static const Hash JUMP_START_STATE = Hash("ACT_STT_JUMP_START");
	static const Hash JUMP_STATE = Hash("ACT_STT_JUMP");
	static const Hash JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
	static const Hash PREPARE_TO_HANG_STATE = Hash("ACT_STT_PREPARE_TO_HANG");
	static const Hash HANG_STATE = Hash("ACT_STT_HANG");
	static const Hash CLIMB_STATE = Hash("ACT_STT_CLIMB");
	static const Hash PREPARE_TO_DESCEND_STATE = Hash("ACT_STT_PREPARE_TO_DESCEND");
	static const Hash DESCEND_STATE = Hash("ACT_STT_DESCEND");

	static const Hash JUMP_INFO_SERIALIZATION = Hash("ACT_SER_JUMP_INFO");
	static const Hash LEDGE_DIRECTED_SERIALIZATION = Hash("ACT_SER_LEDGE_DIRECTED");
	static const VectorSerializer HANG_DESCEND_POINT_SERIALIZATION("ACT_SER_HANG_DESCEND_POINT");

	/**********************************************************************************************
	 * Helpers
	 *********************************************************************************************/
	ActionController& getActionController(StateMachineComponent* stateMachine)
	{
		return *static_cast<ActionController*>(stateMachine);
	}

	JumpInfoProvider::JumpInfoProvider()
	{
		_data[ANGLE_45_IN_RADIANS] = new JumpInfo(JUMP_FORWARD_START_ANIMATION, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);
		_data[ANGLE_60_IN_RADIANS] = new JumpInfo(JUMP_FORWARD_START_ANIMATION, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);
		_data[ANGLE_75_IN_RADIANS] = new JumpInfo(JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
		_data[ANGLE_90_IN_RADIANS] = new JumpInfo(JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
		_data[ANGLE_105_IN_RADIANS] = new JumpInfo(JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	}

	void JumpInfoProvider::dispose() const
	{
		for(JumpInfoIterator i = _data.begin(); i != _data.end(); ++i)
			delete i->second;
	}

	float JumpInfoProvider::getFarthest() const { return ANGLE_45_IN_RADIANS; }
	float JumpInfoProvider::getHighest() const { return ANGLE_90_IN_RADIANS; }

	void HangDescendHelper::setPoint(const SensorCollisionParams& params)
	{
		_point = params.getPoint() == NULL ? Vector::Zero : *params.getPoint();
	}

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
		states[PREPARE_TO_JUMP_STATE] = new PrepareToJump();
		states[JUMP_START_STATE] = new JumpStart();
		states[JUMP_STATE] = new Jump();
		states[JUMP_END_STATE] = new JumpEnd();
		states[PREPARE_TO_HANG_STATE] = new PrepareToHang();
		states[HANG_STATE] = new Hang();
		states[CLIMB_STATE] = new Climb();
		states[PREPARE_TO_DESCEND_STATE] = new PrepareToDescend();
		states[DESCEND_STATE] = new Descend();
		return states;
	}

	Hash ActionController::getInitialState() const { return STAND_STATE; }

	void ActionController::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			serialization.serialize(JUMP_INFO_SERIALIZATION, getJumpHelper().getAngle());
			serialization.serialize(LEDGE_DIRECTED_SERIALIZATION, getJumpHelper().isLedgeDirected());
			HANG_DESCEND_POINT_SERIALIZATION.serialize(serialization, getHangDescendHelper().getPoint());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getSerializationParam(message.getParam());
			getJumpHelper().setAngle(serialization.deserializeFloat(JUMP_INFO_SERIALIZATION));
			getJumpHelper().setLedgeDirected(serialization.deserializeBool(LEDGE_DIRECTED_SERIALIZATION));
			Vector point = Vector::Zero;
			HANG_DESCEND_POINT_SERIALIZATION.deserialize(serialization, point);
			getHangDescendHelper().setPoint(point);
		}
	}

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/
	bool canJumpForward(StateMachineComponent* component)
	{
		const Vector& groundVector = getVectorParam(component->raiseMessage(Message(MessageID::GET_GROUND_VECTOR)));
		Side::Enum orientation = getOrientation(*component);

		return !sameSign(static_cast<float>(orientation), groundVector.getY()) || abs(groundVector.getAngle()) <= ANGLE_30_IN_RADIANS;
	}

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
			getActionController(_stateMachine).getJumpHelper().setAngle(JumpInfoProvider::get().getHighest());
			_stateMachine->changeState(PREPARE_TO_JUMP_STATE);
		}
		// TempFlag1 - Is descending
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if(_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, BACK_EDGE_SENSOR_ID))
		{
			const SensorCollisionParams& params = getSensorCollisionParams(message.getParam());
			getActionController(_stateMachine).getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_DESCEND_STATE);
		}
		else if(_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
		{
			_stateMachine->changeState(TURN_STATE);
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
			_stateMachine->setTempFlag1(true);
		}
		else if (_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			const SensorCollisionParams& params = getSensorCollisionParams(message.getParam());
			getActionController(_stateMachine).getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
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

	// BRODER
	const float Walk::NO_FLOOR_TIME_TO_FALL_IN_MILLIS = 75.0f;

	void Walk::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			if(canJumpForward(_stateMachine))
			{
				getActionController(_stateMachine).getJumpHelper().setAngle(JumpInfoProvider::get().getFarthest());
				_stateMachine->changeState(JUMP_START_STATE);
			}
		}
		// TempFlag 1 - still walking
		// TempFlag 2 - no floor
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if(collision.getY() >= 0.0f)
				_stateMachine->setTempFlag2(true);
		}
		else if(message.getID() == MessageID::UPDATE)
		{			
			if(!_stateMachine->getTempFlag2())
				_stateMachine->getTimer().reset();

			// When climbing on a slope, it is possible to be off the ground for some time when transitioning to a more moderate slope. 
			// Therefore we only fall when some time has passed
			if(_stateMachine->getTimer().getElapsedTimeInMillis() >= NO_FLOOR_TIME_TO_FALL_IN_MILLIS)
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
				// Also, if we're in the midair (explained how it can happen in the previous comment), this will maintain a steady speed
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

	void PrepareToJump::handleJumpSensor(Message &message) const
	{
		const SensorCollisionParams& params = getSensorCollisionParams(message.getParam());
		const Vector* point = params.getPoint();
		Side::Enum orientation = getOrientation(*_stateMachine);
		const AABB& personBounds =  *static_cast<AABB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float target = point->getX();
		float front = personBounds.getSide(orientation);
		float distance = (target - front) * orientation;
		JumpHelper& jumpHelper = getActionController(_stateMachine).getJumpHelper();

		float max = 0.0f;
		const Vector& gravity = DynamicBody::GRAVITY;

		// Broder
		if(distance >= 0 && distance < 20.0f)
		{
			jumpHelper.setAngle(JumpInfoProvider::get().getHighest());
			jumpHelper.setLedgeDirected(true);
			return;
		}
		const JumpInfoCollection& data = JumpInfoProvider::get().getData();
		for(JumpInfoIterator i = data.begin(); i != data.end(); ++i)
		{
			float angle = i->first;
			const JumpInfo* jumpInfo = i->second;
			float height = getJumpHeight(angle, JUMP_FORCE_PER_SECOND, gravity.getY(), distance);
			if(max < height)
			{
				max = height;
				jumpHelper.setAngle(angle);
				jumpHelper.setLedgeDirected(true);
			}
		}
	}

	void PrepareToJump::enter() const
	{
		getActionController(_stateMachine).getJumpHelper().setLedgeDirected(false);
	}

	void PrepareToJump::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_FORWARD && canJumpForward(_stateMachine))
		{
			getActionController(_stateMachine).getJumpHelper().setAngle(JumpInfoProvider::get().getFarthest());
			_stateMachine->changeState(JUMP_START_STATE);
		}
		else if(isSensorCollisionMessage(message, JUMP_SENSOR_ID))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->changeState(JUMP_START_STATE);
		}
	}

	void JumpStart::enter() const
	{
		Hash animation = getActionController(_stateMachine).getJumpHelper().getInfo().getStartAnimation();
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &animation));
	}

	void JumpStart::handleMessage(Message& message) const
	{		
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			JumpHelper& jumpHelper = getActionController(_stateMachine).getJumpHelper();
			if(jumpHelper.getAngle() != JumpInfoProvider::get().getFarthest() && !jumpHelper.isLedgeDirected() && canJumpForward(_stateMachine))
			{
				jumpHelper.setAngle(JumpInfoProvider::get().getFarthest());
				_stateMachine->changeState(JUMP_START_STATE);
			}
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(TURN_STATE);
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			_stateMachine->changeState(JUMP_STATE);
		}
	}

	void Jump::enter() const
	{
		const JumpHelper& jumpHelper = getActionController(_stateMachine).getJumpHelper();
		float angle = jumpHelper.getAngle();
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
		else if (_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			const SensorCollisionParams& params = getSensorCollisionParams(message.getParam());
			getActionController(_stateMachine).getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
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

	void PrepareToHang::update() const
	{
		const AABB& personBounds = *static_cast<AABB*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		const Vector& point = getActionController(_stateMachine).getHangDescendHelper().getPoint();
		float platformTop = point.getY();
		float entityTop = personBounds.getTop();
		float movementY = platformTop - entityTop;

		Side::Enum orientation = getOrientation(*_stateMachine);
		float platformEdge = point.getX();
		float entityFront = personBounds.getSide(orientation);
		float movementX = (platformEdge - entityFront) * orientation;
		Vector movement(movementX, movementY);
		if(movement != Vector::Zero)
		{
			_stateMachine->raiseMessage(Message(MessageID::SET_ABSOLUTE_IMPULSE, &movement));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(HANG_STATE);
		}
	}

	void PrepareToHang::enter() const
	{
		bool gravityEnabled = false;
		_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToHang::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Hang::enter() const
	{
		_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &HANG_ANIMATION));
	}

	void Hang::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, const_cast<Vector*>(&Vector::Zero)));
			_stateMachine->raiseMessage(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(1.0f, -1.0f)));
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
		const Shape& shape = *static_cast<Shape*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		float climbForceX = 1.0f;
		float climbForceY = shape.getHeight();
		Vector climbForce(climbForceX, climbForceY);

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

	void PrepareToDescend::update() const
	{
		Side::Enum orientation = getOrientation(*_stateMachine);
		const Shape& personBounds = *static_cast<Shape*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
		const Vector& point = getActionController(_stateMachine).getHangDescendHelper().getPoint();
		float platformEdge = point.getX();
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		float moveY = point.getY() - personBounds.getBottom();
		Vector movement = Vector(moveX, moveY);
		if(movement != Vector::Zero)
		{
			_stateMachine->raiseMessage(Message(MessageID::SET_ABSOLUTE_IMPULSE, &movement));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = point.getY();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->raiseMessage(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(DESCEND_STATE);
		}
	}

	void PrepareToDescend::enter() const
	{
		bool gravityEnabled = false;
		_stateMachine->raiseMessage(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToDescend::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Descend::enter() const
	{
		const Shape& size = *static_cast<Shape*>(_stateMachine->raiseMessage(Message(MessageID::GET_SHAPE)));
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