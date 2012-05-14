#include "ActionController.h"
#include "Message.h"
#include "MessageParams.h"
#include "MovementUtils.h"
#include "MessageUtils.h"
#include <Temporal\Base\Serialization.h>
#include <Temporal\Base\BaseUtils.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Base\Shape.h>
#include <Temporal\Base\Math.h>
#include <Temporal\Game\MessageParams.h>
#include <Temporal\Physics\DynamicBody.h>

namespace Temporal
{
	/**********************************************************************************************
	 * Constants
	 *********************************************************************************************/
	static const Hash BACK_EDGE_SENSOR_ID = Hash("SNS_BACK_EDGE");
	static const Hash FRONT_EDGE_SENSOR_ID = Hash("SNS_FRONT_EDGE");
	static const Hash JUMP_SENSOR_ID = Hash("SNS_JUMP");
	static const Hash HANG_SENSOR_ID = Hash("SNS_HANG");

	static const Hash STAND_ANIMATION = Hash("POP_ANM_STAND");
	static const Hash TURN_ANIMATION = Hash("POP_ANM_TURN");
	static const Hash DROP_ANIMATION = Hash("POP_ANM_DROP");
	static const Hash FALL_START_ANIMATION = Hash("POP_ANM_FALL_START");
	static const Hash FALL_ANIMATION = Hash("POP_ANM_FALL");
	static const Hash JUMP_UP_START_ANIMATION = Hash("POP_ANM_JUMP_UP_START");
	static const Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP_UP");
	static const Hash HANG_ANIMATION = Hash("POP_ANM_HANG");
	static const Hash CLIMB_ANIMATION = Hash("POP_ANM_CLIMB");
	static const Hash JUMP_FORWARD_START_ANIMATION = Hash("POP_ANM_JUMP_FORWARD_START");
	static const Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP_FORWARD");
	static const Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_JUMP_FORWARD_END");
	static const Hash WALK_ANIMATION = Hash("POP_ANM_WALK");
	static const Hash SWING_FORWARD_ANIMATION = Hash("POP_ANM_SWING_FORWARD");
	static const Hash SWING_BACKWARD_ANIMATION = Hash("POP_ANM_SWING_BACKWARD");

	static const Hash STAND_STATE = Hash("ACT_STT_STAND");
	static const Hash FALL_STATE = Hash("ACT_STT_FALL");
	static const Hash WALK_STATE = Hash("ACT_STT_WALK");
	static const Hash TURN_STATE = Hash("ACT_STT_TURN");
	static const Hash PREPARE_TO_JUMP_STATE = Hash("ACT_STT_PREPARE_TO_JUMP");
	static const Hash JUMP_START_STATE = Hash("ACT_STT_JUMP_START");
	static const Hash JUMP_STATE = Hash("ACT_STT_JUMP");
	static const Hash JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
	static const Hash PREPARE_TO_HANG_STATE = Hash("ACT_STT_PREPARE_TO_HANG");
	static const Hash HANGING_STATE = Hash("ACT_STT_HANGING");
	static const Hash HANG_STATE = Hash("ACT_STT_HANG");
	static const Hash DROP_STATE = Hash("ACT_STT_DROP");
	static const Hash CLIMB_STATE = Hash("ACT_STT_CLIMB");
	static const Hash PREPARE_TO_DESCEND_STATE = Hash("ACT_STT_PREPARE_TO_DESCEND");
	static const Hash DESCEND_STATE = Hash("ACT_STT_DESCEND");

	static const Hash JUMP_INFO_SERIALIZATION = Hash("ACT_SER_JUMP_INFO");
	static const Hash LEDGE_DIRECTED_SERIALIZATION = Hash("ACT_SER_LEDGE_DIRECTED");
	static const NumericPairSerializer HANG_DESCEND_POINT_SERIALIZATION("ACT_SER_HANG_DESCEND_POINT");

	/**********************************************************************************************
	 * Helpers
	 *********************************************************************************************/
	JumpInfoProvider::JumpInfoProvider(void)
	{
		_data.push_back(new JumpInfo(ANGLE_45_IN_RADIANS, JUMP_FORWARD_START_ANIMATION, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION));
		_data.push_back(new JumpInfo(ANGLE_60_IN_RADIANS, JUMP_FORWARD_START_ANIMATION, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION));
		_data.push_back(new JumpInfo(ANGLE_75_IN_RADIANS, JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, STAND_ANIMATION));
		_data.push_back(new JumpInfo(ANGLE_90_IN_RADIANS, JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, STAND_ANIMATION));
		_data.push_back(new JumpInfo(ANGLE_105_IN_RADIANS, JUMP_UP_START_ANIMATION, JUMP_UP_ANIMATION, STAND_ANIMATION));
	}

	JumpInfoProvider::~JumpInfoProvider(void)
	{
		for(JumpInfoIterator i = _data.begin(); i != _data.end(); ++i)
			delete *i;
	}

	void HangDescendHelper::setPoint(const SensorCollisionParams& params)
	{
		_point = params.getPoint() == NULL ? Point::Zero : *params.getPoint();
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
		states[HANGING_STATE] = new Hanging();
		states[HANG_STATE] = new Hang();
		states[DROP_STATE] = new Drop();
		states[CLIMB_STATE] = new Climb();
		states[PREPARE_TO_DESCEND_STATE] = new PrepareToDescend();
		states[DESCEND_STATE] = new Descend();
		return states;
	}

	Hash ActionController::getInitialState(void) const { return STAND_STATE; }

	void ActionController::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(JUMP_INFO_SERIALIZATION, (int)&(getJumpHelper().getInfo()));
			serialization.serialize(LEDGE_DIRECTED_SERIALIZATION, getJumpHelper().isLedgeDirected());
			HANG_DESCEND_POINT_SERIALIZATION.serialize(serialization, getHangDescendHelper().getPoint());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(Serialization*)message.getParam();
			getJumpHelper().setInfo((const JumpInfo*)serialization.deserializeInt(JUMP_INFO_SERIALIZATION));
			getJumpHelper().setLedgeDirected(serialization.deserializeBool(LEDGE_DIRECTED_SERIALIZATION));
			Point point = Point::Zero;
			HANG_DESCEND_POINT_SERIALIZATION.deserialize(serialization, point);
			getHangDescendHelper().setPoint(point);
		}
	}

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/
	bool canJumpForward(StateMachineComponent* component)
	{
		const Vector& groundVector = *(Vector*)component->sendMessageToOwner(Message(MessageID::GET_GROUND_VECTOR));
		Orientation::Enum orientation = *(Orientation::Enum*)component->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

		return !sameSign((float)orientation, groundVector.getVy()) || abs(groundVector.getAngle()) <= ANGLE_30_IN_RADIANS;
	}

	void Stand::enter(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(STAND_ANIMATION)));
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
			((ActionController*)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getHighest());
			_stateMachine->changeState(PREPARE_TO_JUMP_STATE);
		}
		// TempFlag1 - Is descending
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_stateMachine->setTempFlag1(true);
		}
		else if(_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, BACK_EDGE_SENSOR_ID))
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_DESCEND_STATE);
		}
		else if(_stateMachine->getTempFlag1() && isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
		{
			_stateMachine->changeState(TURN_STATE);
		}
	}

	void Fall::enter(void) const
	{
		// Not setting force because we want to continue the momentum
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(FALL_ANIMATION)));
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
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(STAND_STATE);
		}
	}

	void Walk::enter(void) const
	{
		// TempFlag 1 - still walking
		_stateMachine->setTempFlag1(true);
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(WALK_ANIMATION, false, true)));
	}

	// BRODER
	const float Walk::NO_FLOOR_TIME_TO_FALL_IN_MILLIS = 50.0f;

	void Walk::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			if(canJumpForward(_stateMachine))
			{
				((ActionController*)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getFarthest());
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
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() >= 0.0f)
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
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_TIME_BASED_IMPULSE, &force));
			}
		}
	}

	void Turn::enter(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(TURN_ANIMATION)));
	}

	void Turn::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToJump::handleJumpSensor(Message &message) const
	{
		const SensorCollisionParams& sensor = *(SensorCollisionParams*)message.getParam();
		const Point* point = sensor.getPoint();
		Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		AABB personBounds(AABB::Zero);
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		float target = point->getX();
		float front = personBounds.getSide(orientation);
		float distance = (target - front) * orientation;
		JumpHelper& jumpHelper = ((ActionController*)_stateMachine)->getJumpHelper();

		float max = 0.0f;
		const Vector& gravity = DynamicBody::GRAVITY;

		// Broder
		if(distance >= 0 && distance < 20.0f)
		{
			jumpHelper.setInfo(JumpInfoProvider::get().getHighest());
			jumpHelper.setLedgeDirected(true);
			return;
		}
		const JumpInfoCollection& data = JumpInfoProvider::get().getData();
		for(JumpInfoIterator i = data.begin(); i != data.end(); ++i)
		{
			const JumpInfo* jumpInfo = *i;
			float height = getJumpHeight(jumpInfo->getAngle(), JUMP_FORCE_PER_SECOND, gravity.getVy(), distance);
			if(max < height)
			{
				max = height;
				jumpHelper.setInfo(jumpInfo);
				jumpHelper.setLedgeDirected(true);
			}
		}
	}

	void PrepareToJump::enter(void) const
	{
		((ActionController*)_stateMachine)->getJumpHelper().setLedgeDirected(false);
	}

	void PrepareToJump::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_FORWARD && canJumpForward(_stateMachine))
		{
			((ActionController*)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getFarthest());
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

	void JumpStart::enter(void) const
	{
		Hash animation = ((ActionController*)_stateMachine)->getJumpHelper().getInfo().getStartAnimation();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
	}

	void JumpStart::handleMessage(Message& message) const
	{		
		if(message.getID() == MessageID::ACTION_FORWARD)
		{
			JumpHelper& jumpHelper = ((ActionController*)_stateMachine)->getJumpHelper();
			if(jumpHelper.getInfo() != *JumpInfoProvider::get().getFarthest() && !jumpHelper.isLedgeDirected() && canJumpForward(_stateMachine))
			{
				jumpHelper.setInfo(JumpInfoProvider::get().getFarthest());
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

	void Jump::enter(void) const
	{
		const JumpInfo& jumpInfo = ((ActionController*)_stateMachine)->getJumpHelper().getInfo();
		float angle = jumpInfo.getAngle();
		float jumpForceX = JUMP_FORCE_PER_SECOND * cos(angle);
		float jumpForceY = JUMP_FORCE_PER_SECOND * sin(angle);
		Vector jumpVector = Vector(jumpForceX, jumpForceY);
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_TIME_BASED_IMPULSE, &jumpVector));
		Hash animation = jumpInfo.getJumpAnimation();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
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
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(JUMP_END_STATE);
		}
	}

	void JumpEnd::enter(void) const
	{
		Hash animation = ((ActionController*)_stateMachine)->getJumpHelper().getInfo().getEndAnimation();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
	}

	void JumpEnd::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToHang::update(void) const
	{
		AABB personBounds(AABB::Zero);
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Point& point = ((ActionController*)_stateMachine)->getHangDescendHelper().getPoint();
		float platformTop = point.getY();
		float entityTop = personBounds.getTop();
		float movementY = platformTop - entityTop;

		Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		float platformEdge = point.getX();
		float entityFront = personBounds.getSide(orientation);
		float movementX = (platformEdge - entityFront) * orientation;
		Vector movement(movementX, movementY);
		if(movement != Vector::Zero)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &movement));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			Point drawPosition(personCenterX, platformTop);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(HANGING_STATE);
		}
	}

	void PrepareToHang::enter(void) const
	{
		bool gravityEnabled = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToHang::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Hanging::enter(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SWING_BACKWARD_ANIMATION, true)));	
	}

	void Hanging::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(HANG_STATE);
		}
	}

	void Hang::enter(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(HANG_ANIMATION)));
	}

	void Hang::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ACTION_DOWN)
		{	
			_stateMachine->changeState(DROP_STATE);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(CLIMB_STATE);
		}
	}

	void Drop::enter(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(DROP_ANIMATION)));

		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(1.0f, -1.0f)));

		bool gravityEnabled = true;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void Drop::exit(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, (void*)&Point::Zero));
	}

	void Drop::handleMessage(Message& message) const
	{
		// TempFlag 1 - Platform found
		if(isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			_stateMachine->setTempFlag1(true);
		}
		if(message.getID() == MessageID::UPDATE)
		{
			if(!_stateMachine->getTempFlag1())
				_stateMachine->changeState(FALL_STATE);
		}
	}

	void Climb::enter(void) const
	{
		const Size& size = *(Size*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
		float climbForceX = 1.0f;
		float climbForceY = size.getHeight();
		Vector climbForce(climbForceX, climbForceY);

		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CLIMB_ANIMATION)));
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &climbForce));
	}

	void Climb::exit(void) const
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, (void*)&Point::Zero));
		bool gravityEnabled = true;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void Climb::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToDescend::update(void) const
	{
		Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		AABB personBounds(AABB::Zero);
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Point& point = ((ActionController*)_stateMachine)->getHangDescendHelper().getPoint();
		float platformEdge = point.getX();
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		float moveY = point.getY() - personBounds.getBottom();
		Vector movement = Vector(moveX, moveY);
		if(movement != Vector::Zero)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &movement));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = point.getY();
			Point drawPosition(personCenterX, platformTop);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(DESCEND_STATE);
		}
	}

	void PrepareToDescend::enter(void) const
	{
		bool gravityEnabled = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToDescend::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Descend::enter(void) const
	{
		const Size& size = *(Size*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
		float forceX = 0.0f;
		float forceY = -(size.getHeight());

		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(forceX, forceY)));
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CLIMB_ANIMATION, true)));
	}

	void Descend::handleMessage(Message& message) const
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(HANG_STATE);
		}
	}

}