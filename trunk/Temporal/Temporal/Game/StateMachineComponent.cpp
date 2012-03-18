#include "StateMachineComponent.h"
#include "Message.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(std::vector<ComponentState*> states)
		: _states(states), _currentState(NULL), _currentStateID(0)
	{
		for(std::vector<ComponentState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			(**i).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent(void)
	{
		for(std::vector<ComponentState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			delete *i;
	}

	void StateMachineComponent::changeState(int stateID, const void* const param)
	{
		if(_currentState != NULL)
		{
			_currentState->exit();
			sendMessageToOwner(Message(MessageID::STATE_EXITED, &_currentStateID));
		}
		_currentState = _states[stateID];
		_currentStateID = stateID;
		_currentState->enter(param);
		sendMessageToOwner(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			changeState(getInitialState());
		}
		// TODO: Argggh! SLOTH
		else if(_currentState != NULL)
		{
			_currentState->handleMessage(message);
		}
	}
}