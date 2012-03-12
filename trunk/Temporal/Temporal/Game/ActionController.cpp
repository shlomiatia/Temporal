#include "ActionController.h"
#include "Message.h"
#include "MessageParams.h"

#include <Temporal\Base\Math.h>
#include <Temporal\Physics\Sensor.h>
#include <Temporal\Physics\Body.h>

namespace Temporal
{
	JumpInfoProvider::JumpInfoProvider(void)
	{
		_data.push_back(new JumpInfo(ANGLE_45_IN_RADIANS, AnimationID::JUMP_FORWARD_START, AnimationID::JUMP_FORWARD, AnimationID::JUMP_FORWARD_END));
		_data.push_back(new JumpInfo(ANGLE_60_IN_RADIANS, AnimationID::JUMP_FORWARD_START, AnimationID::JUMP_FORWARD, AnimationID::JUMP_FORWARD_END));
		_data.push_back(new JumpInfo(ANGLE_75_IN_RADIANS, AnimationID::JUMP_UP_START, AnimationID::JUMP_UP, AnimationID::STAND));
		_data.push_back(new JumpInfo(ANGLE_90_IN_RADIANS, AnimationID::JUMP_UP_START, AnimationID::JUMP_UP, AnimationID::STAND));
		_data.push_back(new JumpInfo(ANGLE_105_IN_RADIANS, AnimationID::JUMP_UP_START, AnimationID::JUMP_UP, AnimationID::STAND));
	}

	JumpInfoProvider::~JumpInfoProvider(void)
	{
		for(unsigned int i = 0; i < _data.size(); ++i)
			delete _data[i];
	}

	std::vector<ComponentState*> ActionController::getStates() const
	{
		std::vector<ComponentState*> states;
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

	bool ActionState::isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative) const
	{
		if(message.getID() == MessageID::BODY_COLLISION)
		{
			Direction::Enum direction = *(const Direction::Enum* const)message.getParam();
			if(match(direction, positive, negative))
				return true;
		}
		return false;
	}

	bool ActionState::isSensorMessage(Message& message, SensorID::Enum sensorID) const
	{
		if(message.getID() == MessageID::SENSOR_COLLISION)
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			if(sensor.getID() == sensorID)
				return true;
		}
		return NULL;
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
			((ActionController* const)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getHighest());
			_stateMachine->changeState(ActionStateID::PREPARE_TO_JUMP);
		}
		else if(message.getID() == MessageID::ACTION_DOWN)
		{
			_isDescending = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_isDescending = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::STAND)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_isDescending = false;	
		}
		// TODO: Consider creating new state for descending, or querying controller directly INPUT
		if(_isDescending)
		{
			if(isSensorMessage(message, SensorID::BACK_EDGE) != NULL)
			{
				const Sensor& sensor = *(const Sensor* const)message.getParam();
				_stateMachine->changeState(ActionStateID::PREPARE_TO_DESCEND, &sensor);
			}
			else if(isSensorMessage(message, SensorID::FRONT_EDGE) != NULL)
			{
				_stateMachine->changeState(ActionStateID::TURN);
			}
		}
	}

	void Fall::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			// Not setting force because we want to continue the momentum
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::FALL)));
		}
	}

	void Walk::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ACTION_UP)
		{
			((ActionController* const)_stateMachine)->getJumpHelper().setInfo(JumpInfoProvider::get().getFarthest());
			_stateMachine->changeState(ActionStateID::PREPARE_TO_JUMP);
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			_stillWalking = true;
		}
		else if(isBodyCollisionMessage(message, Direction::ALL, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::FALL);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stillWalking = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::WALK, false, true)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_stillWalking)
			{
				_stateMachine->changeState(ActionStateID::STAND);
			}
			else
			{
				// TODO: Move to enter state when there will be walk start state SLOTH!
				Vector force(WALK_FORCE_PER_SECOND, 0.0f);
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &force));
				_stillWalking = false;
			}
		}
	}

	void Turn::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::TURN)));
		}
	}

	void PrepareToJump::handleJumpSensor(Message &message)
	{
		// TODO: Transfer sensor message param with entity id, and query everything ENTITIES
		const Sensor& sensor = *(const Sensor* const)message.getParam();
		const Body* const sensedBody = sensor.getSensedBody();
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		float target = sensedBody->getBounds().getOppositeSide(orientation);
		float front = personBounds.getSide(orientation);
		float x = (target - front) * orientation;
		JumpHelper& jumpHelper = ((ActionController* const)_stateMachine)->getJumpHelper();

		float max = 0.0f;
		const float F = JUMP_FORCE_PER_SECOND;
		const float G = *(const float* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_GRAVITY));

		const std::vector<const JumpInfo* const>& data = JumpInfoProvider::get().getData();
		for(unsigned int i = 0; i < data.size(); ++i)
		{
			/* x = T*F*cos(A)
				* y = T*F*sin(A) - (G*T^2)/2
				*
				* T = x/(F*cos(A))
				* y = (x/(F*cos(A))*F*sin(A) - (G*(x/(F*cos(A)))^2)/2
				* y = (x*sin(A))/(cos(A)) - (G*x^2)/(2*F^2*cos(A)^2)
				* y = (2*F^2*x*sin(A)*cos(A) - G*x^2)/(2*F^2*cos(A)^2)
				* y = x*(2*F^2*sin(A)*cos(A) - G*x)/(2*F^2*cos(A)^2)
				*/
			const JumpInfo* const jumpInfo = data[i];
			float A = jumpInfo->getAngle();
			float y = x*(2.0f*pow(F,2.0f)*sin(A)*cos(A) - G*x)/(2.0f*pow(F,2.0f)*pow(cos(A),2.0f));

			if(max < y)
			{
				max = y;
				jumpHelper.setInfo(jumpInfo);
				jumpHelper.setLedgeDirected(true);
			}
		}
	}

	void PrepareToJump::handleMessage(Message& message)
	{
		if(isSensorMessage(message, SensorID::JUMP))
		{
			handleJumpSensor(message);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			((ActionController* const)_stateMachine)->getJumpHelper().setLedgeDirected(false);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			_stateMachine->changeState(ActionStateID::JUMP_START);
		}
	}

	void JumpStart::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{	
			_animationEnded = true;
		}
		else if(message.getID() == MessageID::ACTION_FORWARD)
		{
			JumpHelper& jumpHelper = ((ActionController* const)_stateMachine)->getJumpHelper();
			if(jumpHelper.getInfo() != *JumpInfoProvider::get().getFarthest() && !jumpHelper.isLedgeDirected())
			{
				jumpHelper.setInfo(JumpInfoProvider::get().getFarthest());
				_stateMachine->changeState(ActionStateID::JUMP_START);
			}
		}
		else if(message.getID() == MessageID::ACTION_BACKWARD)
		{
			_stateMachine->changeState(ActionStateID::TURN);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_animationEnded = false;
			AnimationID::Enum animation = ((ActionController* const)_stateMachine)->getJumpHelper().getInfo().getStartAnimation();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(_animationEnded)
			{
				_stateMachine->changeState(ActionStateID::JUMP);
			}
		}
	}

	void Jump::handleMessage(Message& message)
	{
		if (isSensorMessage(message, SensorID::HANG))
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_stateMachine->changeState(ActionStateID::PREPARE_TO_HANG, &sensor);
		}
		else if(isBodyCollisionMessage(message, Direction::BOTTOM))
		{
			_stateMachine->changeState(ActionStateID::JUMP_END);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			const JumpInfo& jumpInfo = ((ActionController* const)_stateMachine)->getJumpHelper().getInfo();
			float angle = jumpInfo.getAngle();
			float jumpForceX = JUMP_FORCE_PER_SECOND * cos(angle);
			float jumpForceY = JUMP_FORCE_PER_SECOND * sin(angle);
			Vector jumpVector(jumpForceX, jumpForceY);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_FORCE, &jumpVector));
			AnimationID::Enum animation = jumpInfo.getJumpAnimation();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
		}
	}

	void JumpEnd::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			AnimationID::Enum animation = ((ActionController* const)_stateMachine)->getJumpHelper().getInfo().getEndAnimation();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(animation)));
		}
	}

	void PrepareToHang::update(void)
	{
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Rect& platformBounds = _platform->getBounds();
		float platformTop = platformBounds.getTop();
		float entityTop = personBounds.getTop();
		float movementY = platformTop - entityTop;

		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		float platformEdge = platformBounds.getOppositeSide(orientation);
		float entityFront = personBounds.getSide(orientation);
		float movementX = (platformEdge - entityFront) * orientation;
		Vector movement(movementX, movementY);
		if(movement != Vector::Zero)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &movement));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(ActionStateID::HANGING);
		}
	}

	void PrepareToHang::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_platform = sensor.getSensedBody();

			bool gravityEnabled = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Hanging::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::HANG);
		}
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::HANG_BACKWARD, true)));	
		}
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
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::HANG)));
		}
	}

	void Drop::handleMessage(Message& message)
	{
		if(isSensorMessage(message, SensorID::HANG))
		{
			// TODO: Query sensor PHYSICS
			_platformFound = true;
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			_platformFound = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::DROP)));

			bool gravityEnabled = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &Vector::Zero));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!_platformFound)
				_stateMachine->changeState(ActionStateID::FALL);
			else
				_platformFound = false;
		}
	}

	void Climb::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::STAND);
		}
		else if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
			float climbForceX = 1.0f;
			float climbForceY = size.getHeight() - 1.0f;
			Vector climbForce(climbForceX, climbForceY);

			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB)));
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &climbForce));
		}
		else if(message.getID() == MessageID::EXIT_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &Vector::Zero));
			bool gravityEnabled = true;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
	}

	void PrepareToDescend::update(void)
	{
		Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Rect personBounds(Vector::Zero, Vector(1.0f, 1.0f));
		_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &personBounds));
		const Rect& platformBounds = _platform->getBounds();
		float platformEdge = platformBounds.getOppositeSide(orientation) + 1.0f * orientation;
		float entityFront = personBounds.getSide(orientation);
		float moveX = (platformEdge - entityFront) * orientation;
		if(moveX != 0.0f)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &Vector(moveX, 0.0f)));
		}
		else
		{
			float personCenterX = personBounds.getCenterX();
			float platformTop = platformBounds.getTop();
			Vector drawPosition(personCenterX, platformTop);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_DRAW_POSITION_OVERRIDE, &drawPosition));
			_stateMachine->changeState(ActionStateID::DESCEND);
		}
	}

	void PrepareToDescend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Sensor& sensor = *(const Sensor* const)message.getParam();
			_platform = sensor.getSensedBody();

			bool gravityEnabled = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_GRAVITY_ENABLED, &gravityEnabled));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
	}

	void Descend::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			const Vector& size = *(const Vector* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_SIZE));
			float forceX = -1.0f;
			float forceY = -(size.getHeight() - 1.0f);

			_stateMachine->sendMessageToOwner(Message(MessageID::SET_IMPULSE, &Vector(forceX, forceY)));
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(AnimationID::CLIMB, true)));
		}
		else if(message.getID() == MessageID::ANIMATION_ENDED)
		{
			_stateMachine->changeState(ActionStateID::HANG);
		}
	}

}