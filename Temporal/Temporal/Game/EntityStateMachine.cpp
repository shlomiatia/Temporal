#include <Temporal\Base\Base.h>
#include "EntityStateMachine.h"
#include "EntityStates.h"
#include "JumpAngles.h"
#include <assert.h>

namespace Temporal
{
	const float EntityStateMachine::WALK_SPEED_PER_SECOND(120.0f);
	const float EntityStateMachine::JUMP_FORCE(15.0f);

	EntityStateMachine::EntityStateMachine(void)
		: _currentState(NULL), _drawPositionOverride(Vector::Zero)
	{
		_states.push_back(new Stand());
		_states.push_back(new Fall());
		_states.push_back(new Walk());
		_states.push_back(new Turn());
		_states.push_back(new PrepareToJump());
		_states.push_back(new JumpStart(DEGREES_45, AnimationID::JUMP_FORWARD_START, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(DEGREES_60, AnimationID::JUMP_FORWARD_START, EntityStateID::JUMP_FORWARD));
		_states.push_back(new JumpStart(DEGREES_75, AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(DEGREES_90, AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
		_states.push_back(new JumpStart(DEGREES_105, AnimationID::JUMP_UP_START, EntityStateID::JUMP_UP));
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
		
		for(std::vector<EntityState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			(**i).setStateMachine(this);
		_currentState = _states[EntityStateID::STAND];
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
			_currentState->handleMessage(Message(MessageID::EXIT_STATE, &stateType));
		}
		_currentState = _states[stateType];
		_currentState->handleMessage(Message(MessageID::ENTER_STATE, param));
	}

	void EntityStateMachine::handleMessage(Message& message)
	{
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
