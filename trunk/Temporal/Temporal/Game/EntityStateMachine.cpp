#include <Temporal\Base\Base.h>
#include "EntityStateMachine.h"
#include "EntityStates.h"
#include <assert.h>

namespace Temporal
{
	// TODO: Switch case
	EntityStateMachine::EntityStateMachine(EntityStateID::Type initialState)
		: _currentState(NULL)
	{
		_states.push_back(new Stand());
		_states.push_back(new Fall());
		_states.push_back(new Walk());
		_states.push_back(new Turn());
		_states.push_back(new PrepareToJump());
		_states.push_back(new JumpStart(toRadians(45), 9, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(toRadians(60), 9, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(toRadians(75), 5, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(toRadians(90), 5, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(toRadians(105), 5, EntityStateID::JUMP_UP));
		_states.push_back(new JumpUp());
		_states.push_back(new JumpForward());
		_states.push_back(new JumpForwardEnd());
		_states.push_back(new Hanging());
		_states.push_back(new Hang());
		_states.push_back(new Drop());
		_states.push_back(new Climbe());
		_states.push_back(new PrepareToDescend());
		_states.push_back(new Descend());
		
		_currentState = _states[initialState];
	}

	EntityStateMachine::~EntityStateMachine(void)
	{
		for(std::vector<EntityState*>::iterator i = _states.begin(); i != _states.end(); ++i)
		{
			delete *i;
		}
	}

	void EntityStateMachine::changeState(EntityStateID::Type stateType, const void* const param)
	{
		if(_currentState != NULL)
		{
			_currentState->handleMessage(*this, Message(MessageID::EXIT_STATE));
		}
		_currentState = _states[stateType];
		_currentState->handleMessage(*this, Message(MessageID::ENTER_STATE, param));
	}

	void EntityStateMachine::handleMessage(Message& message)
	{
		_currentState->handleMessage(*this, message);
	}
}
