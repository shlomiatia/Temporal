#include <Temporal\Base\Base.h>
#include "EntityStateMachine.h"
#include "EntityStates.h"
#include <assert.h>

namespace Temporal
{
	const float EntityStateMachine::WALK_FORCE(2.0f);
	const float EntityStateMachine::JUMP_FORCE(15.0f);

	EntityStateMachine::EntityStateMachine(EntityStateID::Enum initialState)
		: _currentState(NULL), _drawPositionOverride(Vector::Zero)
	{
		_states.push_back(new Stand(*this));
		_states.push_back(new Fall(*this));
		_states.push_back(new Walk(*this));
		_states.push_back(new Turn(*this));
		_states.push_back(new PrepareToJump(*this));
		_states.push_back(new JumpStart(*this, toRadians(45), AnimationID::JUMP_FORWARD_START, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(*this, toRadians(60), AnimationID::JUMP_FORWARD_START, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(*this, toRadians(75), AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(*this, toRadians(90), AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(*this, toRadians(105), AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
		_states.push_back(new JumpUp(*this));
		_states.push_back(new JumpForward(*this));
		_states.push_back(new JumpForwardEnd(*this));
		_states.push_back(new Hanging(*this));
		_states.push_back(new Hang(*this));
		_states.push_back(new Drop(*this));
		_states.push_back(new Climbe(*this));
		_states.push_back(new PrepareToDescend(*this));
		_states.push_back(new Descend(*this));
		
		_currentState = _states[initialState];
	}

	EntityStateMachine::~EntityStateMachine(void)
	{
		for(std::vector<EntityState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			delete *i;
	}

	void EntityStateMachine::changeState(EntityStateID::Enum stateType, const void* const param)
	{
		if(_currentState != NULL)
		{
			_currentState->handleMessage(Message(MessageID::EXIT_STATE));
		}
		_currentState = _states[stateType];
		_currentState->handleMessage(Message(MessageID::ENTER_STATE, param));
	}

	void EntityStateMachine::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_DRAW_POSITION)
		{
			if(_drawPositionOverride != Vector::Zero)
			{
				message.setOutParam(_drawPositionOverride);
			}
			else
			{
				Rect bounds(Vector::Zero, Vector(1.0f, 1.0f));
				sendMessageToOwner(Message(MessageID::GET_BOUNDS, &bounds));
				Vector drawPosition(bounds.getCenterX(), bounds.getBottom());
				message.setOutParam(drawPosition);
			}
		}
		else
		{
			_currentState->handleMessage(message);
		}
	}
}
