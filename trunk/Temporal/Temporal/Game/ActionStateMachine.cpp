#include <Temporal\Base\Base.h>
#include "ActionStateMachine.h"
#include "ActionStates.h"
#include "JumpAngles.h"

namespace Temporal
{
	const float ActionStateMachine::WALK_FORCE_PER_SECOND(120.0f);
	const float ActionStateMachine::JUMP_FORCE_PER_SECOND(1000.0f);

	ActionStateMachine::ActionStateMachine(void)
		: _currentState(NULL), _drawPositionOverride(Vector::Zero)
	{
		_states.push_back(new Stand());
		_states.push_back(new Fall());
		_states.push_back(new Walk());
		_states.push_back(new Turn());
		_states.push_back(new PrepareToJump());
		_states.push_back(new JumpStart(DEGREES_45, AnimationID::JUMP_FORWARD_START, ActionStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(DEGREES_60, AnimationID::JUMP_FORWARD_START, ActionStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(DEGREES_75, AnimationID::JUMP_UP_START, ActionStateID::JUMP_UP));
		_states.push_back(new JumpStart(DEGREES_90, AnimationID::JUMP_UP_START, ActionStateID::JUMP_UP));
		_states.push_back(new JumpStart(DEGREES_105, AnimationID::JUMP_UP_START, ActionStateID::JUMP_UP));
		_states.push_back(new JumpUp());
		_states.push_back(new JumpForward());
		_states.push_back(new JumpForwardEnd());
		_states.push_back(new PrepareToHang());
		_states.push_back(new Hanging());
		_states.push_back(new Hang());
		_states.push_back(new Drop());
		_states.push_back(new Climb());
		_states.push_back(new PrepareToDescend());
		_states.push_back(new Descend());
		
		for(std::vector<ActionState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			(**i).setStateMachine(this);

		// TODO: Use change state
		_currentState = _states[ActionStateID::STAND];
	}

	ActionStateMachine::~ActionStateMachine(void)
	{
		for(std::vector<ActionState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			delete *i;
	}

	void ActionStateMachine::changeState(ActionStateID::Enum stateType, const void* const param)
	{
		if(_currentState != NULL)
		{
			_currentState->handleMessage(Message(MessageID::EXIT_STATE));
		}
		_currentState = _states[stateType];
		_currentState->handleMessage(Message(MessageID::ENTER_STATE, param));
	}

	void ActionStateMachine::handleMessage(Message& message)
	{
		// TODO: Shit
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			Vector* outParam = (Vector* const)message.getParam();
			if(_drawPositionOverride != Vector::Zero)
			{
				*outParam = _drawPositionOverride;
			}
			else
			{
				Rect bounds(Vector::Zero, Vector(1.0f, 1.0f));
				sendMessageToOwner(Message(MessageID::GET_BOUNDS, &bounds));
				*outParam = Vector(bounds.getCenterX(), bounds.getBottom());
			}
		}
		else
		{
			_currentState->handleMessage(message);
		}
	}
}
