#include "AIStateMachine.h"

namespace Temporal
{
	AIStateMachine::AIStateMachine(std::vector<AIState*> states)
		: _states(states), _currentState(NULL)
	{
		for(std::vector<AIState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			(**i).setStateMachine(this);
	}

	AIStateMachine::~AIStateMachine(void)
	{
		for(std::vector<AIState*>::iterator i = _states.begin(); i != _states.end(); ++i)
			delete *i;
	}

	void AIStateMachine::changeState(int stateType)
	{
		if(_currentState != NULL)
		{
			_currentState->handleMessage(Message(MessageID::EXIT_STATE));
		}
		_currentState = _states[stateType];
		_currentState->handleMessage(Message(MessageID::ENTER_STATE));
	}

	void AIStateMachine::handleMessage(Message& message)
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
