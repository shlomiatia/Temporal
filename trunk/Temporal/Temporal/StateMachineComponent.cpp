#include "StateMachineComponent.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(StateCollection states, const char* prefix)
		: _states(states), _currentState(0), _currentStateID(Hash::INVALID)
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			(*(*i).second).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent()
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (*i).second;
	}

	void StateMachineComponent::changeState(Hash stateID)
	{
		if(_currentState)
		{
			_currentState->exit();
			raiseMessage(Message(MessageID::STATE_EXITED, &_currentStateID));
		}
		setState(stateID);
		_currentState->enter();
		raiseMessage(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(_currentState)
			_currentState->handleMessage(message);
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			setState(getInitialState());
			_currentState->enter();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			_timer.update(framePeriod);
			resetFrameFlags();
		}
	}

	void StateMachineComponent::setState(Hash stateID)
	{
		_currentState = _states[stateID];
		_currentStateID = stateID;
		resetFrameFlags();
		_timer.reset();
		_stateFlag = false;
	}
}