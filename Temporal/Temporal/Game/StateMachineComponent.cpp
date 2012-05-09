#include "StateMachineComponent.h"
#include "Message.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(StateCollection states)
		: _states(states), _currentState(NULL), _currentStateID(Hash::INVALID)
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			(*(*i).second).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent(void)
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (*i).second;
	}

	void StateMachineComponent::changeState(const Hash& stateID)
	{
		if(_currentState != NULL)
		{
			_currentState->exit();
			sendMessageToOwner(Message(MessageID::STATE_EXITED, &_currentStateID));
		}
		_currentState = _states[stateID];
		_currentStateID = stateID;
		_currentState->enter();
		sendMessageToOwner(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			changeState(getInitialState());
		}
		// Protect against events that occur before the inital state is set
		else if(_currentState != NULL)
		{
			_currentState->handleMessage(message);
		}
	}
}