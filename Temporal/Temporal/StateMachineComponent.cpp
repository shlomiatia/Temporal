#include "StateMachineComponent.h"
#include "MessageUtils.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(HashStateMap states, const char* prefix, Hash initialState)
		: _states(states), _currentState(0), _currentStateID(initialState), _stateFlag(false), _frameFlag1(false), _frameFlag2(false), _frameFlag3(false)
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			(*(*i).second).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent()
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (*i).second;
	}

	void StateMachineComponent::changeState(Hash stateID, void* param)
	{
		if(_currentState)
		{
			_currentState->exit(param);
			raiseMessage(Message(MessageID::STATE_EXITED, &_currentStateID));
		}
		resetState(stateID);
		_currentState->enter(param);
		raiseMessage(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			if (_currentStateID == Hash::INVALID)
				resetState(getInitialState());
			else
				resetState(_currentStateID);
			_currentState->enter(0);
		}
		else if(message.getID() == MessageID::POST_LOAD)
		{
			setState(_currentStateID);
		}

		if (_currentState)
			_currentState->handleMessage(message);

		if (message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			_timer.update(framePeriod);
			resetFrameFlags();
		}

	}

	void StateMachineComponent::resetState(Hash stateID)
	{
		setState(stateID);
		_timer.reset();
		_stateFlag = false;
	}

	void StateMachineComponent::setState(Hash stateID)
	{
		_currentState = _states[stateID];
		_currentStateID = stateID;
		resetFrameFlags();
	}
}