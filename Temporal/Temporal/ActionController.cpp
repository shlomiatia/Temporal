#include "ActionController.h"
#include "ActionControllerStand.h"
#include "ActionControllerFall.h"
#include "ActionControllerWalk.h"
#include "ActionControllerTurn.h"
#include "ActionControllerJump.h"
#include "ActionControllerJumpEnd.h"
#include "ActionControllerHang.h"
#include "ActionControllerClimb.h"
#include "ActionControllerDescend.h"
#include "ActionControllerTakedown.h"
#include "ActionControllerDie.h"
#include "ActionControllerAim.h"
#include "ActionControllerDead.h"
#include "ActionControllerFire.h"
#include "ActionControllerDragStand.h"
#include "ActionControllerDragForward.h"
#include "ActionControllerDragBackwards.h"
#include "ActionControllerInvestigate.h"
#include "Ids.h"
#include "MessageUtils.h"

namespace Temporal
{
	float ActionController::WALK_ACC_PER_SECOND(1250.0f);
	float ActionController::JUMP_FORCE_PER_SECOND(400.0f);
	float ActionController::FALL_ALLOW_JUMP_TIME(0.1f);
	float ActionController::JUMP_STOP_MODIFIER(0.5f);
	float ActionController::MAX_WALK_JUMP_MODIFIER(1.15f);

	ActionController& ActionController::getActionController(StateMachineComponent* stateMachine)
	{
		return *static_cast<ActionController*>(stateMachine);
	}

	ActionController::ActionController(float maxWalkForcePerSecond, Hash initialStateId) :
		StateMachineComponent(getStates(), "ACT", initialStateId), _ledgeDetector(SensorIds::LEDGE_SENSOR_ID, *this), _handleMessageHelper(*this), _dragMessagesHandler(*this),
		_maxWalkForcePerSecond(maxWalkForcePerSecond), _isInvestigated(false) {}

	Hash ActionController::getInitialState() const
	{
		return ActionControllerStateIds::FALL_STATE;
	}

	void ActionController::handleMessage(Message& message)
	{
		_ledgeDetector.handleMessage(message);
		StateMachineComponent::handleMessage(message);
		
		if (message.getID() == MessageID::INVESTIGATE)
		{
			_isInvestigated = true;
		}
		else if (message.getID() == MessageID::IS_INVESTIGATED)
		{
			message.setParam(&_isInvestigated);
		}
		else if (message.getID() == MessageID::DIE)
		{
			die();
		}
		else if (message.getID() == MessageID::COLLIDED_BY_ENTITY)
		{
			const Vector& collision = getVectorParam(message.getParam());
			if (collision.getX() == 0.0f && collision.getY() > 0.0f)
			{
				die();
			}
		}
		else if (message.getID() == MessageID::LOST_GROUND)
		{
			if (getCurrentStateID() != ActionControllerStateIds::FALL_STATE &&
				getCurrentStateID() != ActionControllerStateIds::JUMP_STATE && 
				getCurrentStateID() != ActionControllerStateIds::DEAD_STATE)
			{
				changeState(ActionControllerStateIds::FALL_STATE);
			}
		}
	}

	void ActionController::die()
	{
		if (getCurrentStateID() != ActionControllerStateIds::DIE_STATE)
		{
			changeState(ActionControllerStateIds::DIE_STATE);
		}
	}

	HashStateMap ActionController::getStates() const
	{
		using namespace ActionControllerStates;
		HashStateMap states;
		states[ActionControllerStateIds::STAND_STATE] = new Stand();
		states[ActionControllerStateIds::FALL_STATE] = new Fall();
		states[ActionControllerStateIds::WALK_STATE] = new Walk();
		states[ActionControllerStateIds::TURN_STATE] = new Turn();
		states[ActionControllerStateIds::JUMP_STATE] = new Jump();
		states[ActionControllerStateIds::JUMP_END_STATE] = new JumpEnd();
		states[ActionControllerStateIds::HANG_STATE] = new Hang();
		states[ActionControllerStateIds::CLIMB_STATE] = new Climb();
		states[ActionControllerStateIds::DESCEND_STATE] = new Descend();
		states[ActionControllerStateIds::TAKEDOWN_STATE] = new Takedown();
		states[ActionControllerStateIds::DIE_STATE] = new Die();
		states[ActionControllerStateIds::DEAD_STATE] = new Dead();
		states[ActionControllerStateIds::AIM_STATE] = new Aim();
		states[ActionControllerStateIds::FIRE_STATE] = new Fire();
		states[ActionControllerStateIds::DRAG_STAND_STATE] = new DragStand();
		states[ActionControllerStateIds::DRAG_FORWARD_STATE] = new DragForward();
		states[ActionControllerStateIds::DRAG_BACKWARDS_STATE] = new DragBackwards();
		states[ActionControllerStateIds::INVESTIGATE_STATE] = new Investigate();
		return states;
	}
}
