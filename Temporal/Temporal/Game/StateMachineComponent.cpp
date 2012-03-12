#include "StateMachineComponent.h"
#include "Message.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(std::vector<ComponentState*> states)
		: _states(states), _currentState(NULL)
	{
		for(std::vector<ComponentState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			(**i).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent(void)
	{
		for(std::vector<ComponentState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			delete *i;
	}

	void StateMachineComponent::changeState(int state, const void* const param)
	{
		if(_currentState != NULL)
		{
			_currentState->handleMessage(Message(MessageID::EXIT_STATE));
		}
		_currentState = _states[state];
		_currentState->handleMessage(Message(MessageID::ENTER_STATE, param));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			changeState(getInitialState());
		}
		else
		{
			_currentState->handleMessage(message);
		}
	}
}