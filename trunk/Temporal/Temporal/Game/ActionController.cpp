#include "ActionController.h"
#include "Message.h"
#include "MessageParams.h"
#include "MovementUtils.h"
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Base\Shape.h>
#include <Temporal\Base\Math.h>
#include <Temporal\Game\MessageParams.h>
#include <Temporal\Physics\DynamicBody.h>

namespace Temporal
{
	static const Hash BACK_EDGE_SENSOR_ID = Hash("SENS_BACK_EDGE");
	static const Hash FRONT_EDGE_SENSOR_ID = Hash("SENS_FRONT_EDGE");
	static const Hash JUMP_SENSOR_ID = Hash("SENS_JUMP");
	static const Hash HANG_SENSOR_ID = Hash("SENS_HANG");

	static const Hash STAND_ANIMATION = Hash("ANIM_POP_STAND");
	static const Hash TURN_ANIMATION = Hash("ANIM_POP_TURN");
	static const Hash DROP_ANIMATION = Hash("ANIM_POP_DROP");
	static const Hash FALL_START_ANIMATION = Hash("ANIM_POP_FALL_START");
	static const Hash FALL_ANIMATION = Hash("ANIM_POP_FALL");
	static const Hash JUMP_UP_START_ANIMATION = Hash("ANIM_POP_JUMP_UP_START");
	static const Hash JUMP_UP_ANIMATION = Hash("ANIM_POP_JUMP_UP");
	static const Hash HANG_ANIMATION = Hash("ANIM_POP_HANG");
	static const Hash CLIMB_ANIMATION = Hash("ANIM_POP_CLIMB");
	static const Hash JUMP_FORWARD_START_ANIMATION = Hash("ANIM_POP_JUMP_FORWARD_START");
	static const Hash JUMP_FORWARD_ANIMATION = Hash("ANIM_POP_JUMP_FORWARD");
	static const Hash JUMP_FORWARD_END_ANIMATION = Hash("ANIM_POP_JUMP_FORWARD_END");
	static const Hash WALK_ANIMATION = Hash("ANIM_POP_WALK");
	static const Hash SWING_FORWARD_ANIMATION = Hash("ANIM_POP_SWING_FORWARD");
	static const Hash SWING_BACKWARD_ANIMATION = Hash("ANIM_POP_SWING_BACKWARD");

	static const Hash STAND_STATE = Hash("STAT_ACT_STAND");
	static const Hash FALL_STATE = Hash("STAT_ACT_FALL");
	static const Hash WALK_STATE = Hash("STAT_ACT_WALK");
	static const Hash TURN_STATE = Hash("STAT_ACT_TURN");
	static const Hash PREPARE_TO_JUMP_STATE = Hash("STAT_ACT_PREPARE_TO_JUMP");
	static const Hash JUMP_START_STATE = Hash("STAT_ACT_JUMP_START");
	static const Hash JUMP_STATE = Hash("STAT_ACT_JUMP");
	static const Hash JUMP_END_STATE = Hash("STAT_ACT_JUMP_END");
	static const Hash PREPARE_TO_HANG_STATE = Hash("STAT_ACT_PREPARE_TO_HANG");
	static const Hash HANGING_STATE = Hash("STAT_ACT_HANGING");
	static const Hash HANG_STATE = Hash("STAT_ACT_HANG");
	static const Hash DROP_STATE = Hash("STAT_ACT_DROP");
	static const Hash CLIMB_STATE = Hash("STAT_ACT_CLIMB");
	static const Hash PREPARE_TO_DESCEND_STATE = Hash("STAT_ACT_PREPARE_TO_DESCEND");
	static const Hash DESCEND_STATE = Hash("STAT_ACT_DESCEND");

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
		_point = params.getPoint();
	}

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

	bool isSensorCollisionMessage(Message& message, const Hash& sensorID)
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			if(params.getSensorID() == sensorID)
				return true;
		}
		return false;
	}

	bool canJumpForward(StateMachineComponent* component)
	{
		const Vector& groundVector = *(Vector*)component->sendMessageToOwner(Message(MessageID::GET_GROUND_VECTOR));
		Orientation::Enum orientation = *(Orientation::Enum*)component->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

		return !sameSign((float)orientation, groundVector.getVy()) || abs(groundVector.getAngle()) <= ANGLE_30_IN_RADIANS;
	}

	void Stand::enter(void)
	{
		_isDescending = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(STAND_ANIMATION)));
	}

	void Stand::handleMessage(Message& message)
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
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = false;	
		}
		else if(_isDescending && isSensorCollisionMessage(message, BACK_EDGE_SENSOR_ID))
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_DESCEND_STATE);
		}
		else if(_isDescending && isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
		{
			_stateMachine->changeState(TURN_STATE);
		}

	}

	void Fall::enter(void)
	{
		// Not setting force because we want to continue the momentum
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(FALL_ANIMATION)));
	}

	void Fall::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			_wantToHang = true;
		}
		else if (_wantToHang && isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_wantToHang = false;
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(STAND_STATE);
		}
	}

	void Walk::enter(void)
	{
		_stillWalking = true;
		_noFloor = false;
		_noFloorTimer.reset();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(WALK_ANIMATION, false, true)));
	}

	// BRODER
	const float Walk::NO_FLOOR_TIME_TO_FALL_IN_MILLIS = 50.0f;

	void Walk::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			if(canJumpForward(_stateMachine))
			{
				((ActionController*)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getFarthest());
				_stateMachine->changeState(JUMP_START_STATE);
			}
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stillWalking = true;
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() >= 0.0f)
				_noFloor = true;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();
			
			if(_noFloor)
				_noFloorTimer.update(framePeriodInMillis);
			else
				_noFloorTimer.reset();
			_noFloor = false;

			// When climbing on a slope, it is possible to be off the ground for some time when transitioning to a more moderate slope. 
			// Therefore we only fall when some time has passed
			if(_noFloorTimer.getElapsedTimeInMillis() >= NO_FLOOR_TIME_TO_FALL_IN_MILLIS)
			{
				_stateMachine->changeState(FALL_STATE);
			}
			else if(!_stillWalking)
			{
				_stateMachine->changeState(STAND_STATE);
			}
			else
			{
				// We need to apply this every update because the ground has infinite restitution. 
				// Also, if we're in the midair (explained how it can happen in the previous comment), this will maintain a steady speed
				Vector force = Vector(WALK_FORCE_PER_SECOND, 0.0f);
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_TIME_BASED_IMPULSE, &force));
				_stillWalking = false;
			}
		}
	}

	void Turn::enter(void)
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(TURN_ANIMATION)));
	}

	void Turn::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToJump::handleJumpSensor(Message &message)
	{
		const SensorCollisionParams& sensor = *(SensorCollisionParams*)message.getParam();
		const Point* point = sensor.getPoint();
		Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		AABB personBounds(AABB::Empty);
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

	void PrepareToJump::enter(void)
	{
		((ActionController*)_stateMachine)->getJumpHelper().setLedgeDirected(false);
	}

	void PrepareToJump::handleMessage(Message& message)
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

	void JumpStart::enter(void)
	{
		_animationEnded = false;
		Hash animation = ((ActionController*)_stateMachine)->getJumpHelper().getInfo().getStartAnimation();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
	}

	void JumpStart::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			_animationEnded = true;
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
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
		else if(message.getID() == MessageID::UPDATE)
		{
			if(_animationEnded)
			{
				_stateMachine->changeState(JUMP_STATE);
			}
		}
	}

	void Jump::enter(void)
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

	void Jump::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			_wantToHang = true;
		}
		else if (_wantToHang && isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			const SensorCollisionParams& params = *(SensorCollisionParams*)message.getParam();
			((ActionController*)_stateMachine)->getHangDescendHelper().setPoint(params);
			_stateMachine->changeState(PREPARE_TO_HANG_STATE);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_wantToHang = false;
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(JUMP_END_STATE);
		}
	}

	void JumpEnd::enter(void)
	{
		Hash animation = ((ActionController*)_stateMachine)->getJumpHelper().getInfo().getEndAnimation();
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
	}

	void JumpEnd::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToHang::update(void)
	{
		AABB personBounds(AABB::Empty);
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

	void PrepareToHang::enter(void)
	{
		bool gravityEnabled = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToHang::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Hanging::enter(void)
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SWING_BACKWARD_ANIMATION, true)));	
	}

	void Hanging::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(HANG_STATE);
		}
	}

	void Hang::enter(void)
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(HANG_ANIMATION)));
	}

	void Hang::handleMessage(Message& message)
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

	void Drop::enter(void)
	{
		_platformFound = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(DROP_ANIMATION)));

		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(1.0f, -1.0f)));

		bool gravityEnabled = true;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void Drop::exit(void)
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, (void*)&Point::Zero));
	}

	void Drop::handleMessage(Message& message)
	{
		if(isSensorCollisionMessage(message, HANG_SENSOR_ID))
		{
			_platformFound = true;
		}
		if(message.getID() == MessageID::UPDATE)
		{
			if(!_platformFound)
				_stateMachine->changeState(FALL_STATE);
			else
				_platformFound = false;
		}
	}

	void Climb::enter(void)
	{
		const Size& size = *(Size*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
		float climbForceX = 1.0f;
		float climbForceY = size.getHeight();
		Vector climbForce(climbForceX, climbForceY);

		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CLIMB_ANIMATION)));
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &climbForce));
	}

	void Climb::exit(void)
	{
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, (void*)&Point::Zero));
		bool gravityEnabled = true;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void Climb::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(STAND_STATE);
		}
	}

	void PrepareToDescend::update(void)
	{
		Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		AABB personBounds(AABB::Empty);
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

	void PrepareToDescend::enter(void)
	{
		bool gravityEnabled = false;
		_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Descend::enter(void)
	{
		const Size& size = *(Size*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
		float forceX = 0.0f;
		float forceY = -(size.getHeight());

		_stateMachine->sendMessageToOwner(Message(MessageID::SET_ABSOLUTE_IMPULSE, &Vector(forceX, forceY)));
		_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CLIMB_ANIMATION, true)));
	}

	void Descend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(HANG_STATE);
		}
	}

}