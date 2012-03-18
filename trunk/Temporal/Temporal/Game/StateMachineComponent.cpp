#include "StateMachineComponent.h"
#include "Message.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(std::vector<ComponentState*> states)
		: _states(states), _currentState(NULL), _currentID(0)
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
			// TODO: Enter
			sendMessageToOwner(Message(MessageID::EXIT_STATE, &_currentID));
		}
		_currentState = _states[state];
		_currentID = state;
		sendMessageToOwner(Message(MessageID::ENTER_STATE, param));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			changeState(getInitialState());
		}
		// TODO: Argggh!
		else if(_currentState != NULL)
		{
			_currentState->handleMessage(message);
		}
	}
}