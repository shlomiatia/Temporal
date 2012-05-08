#include "ActionController.h"
#include "Message.h"
#include "MessageParams.h"
#include "MovementUtils.h"
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Base\Shape.h>
#include <Temporal\Base\Math.h>
#include <Temporal\Game\MessageParams.h>

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
		states.push_back(new Stand());
		states.push_back(new Fall());
		states.push_back(new Walk());
		states.push_back(new Turn());
		states.push_back(new PrepareToJump());
		states.push_back(new JumpStart());
		states.push_back(new Jump());
		states.push_back(new JumpEnd());
		states.push_back(new PrepareToHang());
		states.push_back(new Hanging());
		states.push_back(new Hang());
		states.push_back(new Drop());
		states.push_back(new Climb());
		states.push_back(new PrepareToDescend());
		states.push_back(new Descend());
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
			_stateMachine->changeState(ActionStateID::WALK);
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(ActionStateID::TURN);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{
			((ActionController*)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getHighest());
			_stateMachine->changeState(ActionStateID::PREPARE_TO_JUMP);
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
			_stateMachine->changeState(ActionStateID::PREPARE_TO_DESCEND);
		}
		else if(_isDescending && isSensorCollisionMessage(message, FRONT_EDGE_SENSOR_ID))
		{
			_stateMachine->changeState(ActionStateID::TURN);
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
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_wantToHang = false;
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(ActionStateID::STAND);
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
				_stateMachine->changeState(ActionStateID::JUMP_START);
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
				_stateMachine->changeState(ActionStateID::FALL);
			}
			else if(!_stillWalking)
			{
				_stateMachine->changeState(ActionStateID::STAND);
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
			_stateMachine->changeState(ActionStateID::STAND);
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
		const Vector& gravity = *(Vector*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_GRAVITY));

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
			_stateMachine->changeState(ActionStateID::JUMP_START);
		}
		else if(isSensorCollisionMessage(message, JUMP_SENSOR_ID))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->changeState(ActionStateID::JUMP_START);
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
				_stateMachine->changeState(ActionStateID::JUMP_START);
			}
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(ActionStateID::TURN);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(_animationEnded)
			{
				_stateMachine->changeState(ActionStateID::JUMP);
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
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_wantToHang = false;
		}
		else if(message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = *(Vector*)message.getParam();
			if(collision.getVy() < 0.0f)
				_stateMachine->changeState(ActionStateID::JUMP_END);
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
			_stateMachine->changeState(ActionStateID::STAND);
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
			_stateMachine->changeState(ActionStateID::HANGING);
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
			_stateMachine->changeState(ActionStateID::HANG);
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
			_stateMachine->changeState(ActionStateID::DROP);
		}
		else if(message.getID() == MessageID::ACTION_UP)
		{	
			_stateMachine->changeState(ActionStateID::CLIMB);
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
				_stateMachine->changeState(ActionStateID::FALL);
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
			_stateMachine->changeState(ActionStateID::STAND);
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
			_stateMachine->changeState(ActionStateID::DESCEND);
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
			_stateMachine->changeState(ActionStateID::HANG);
		}
	}

}