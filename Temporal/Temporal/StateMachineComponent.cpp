#include "StateMachineComponent.h"
#include "Serialization.h"
#include "MessageUtils.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(StateCollection states, const char* prefix)
		: _states(states), _currentState(0), _currentStateID(Hash::INVALID), _stateFlag(false), _frameFlag1(false), _frameFlag2(false), _frameFlag3(false)
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
		setState(stateID);
		_currentState->enter(param);
		raiseMessage(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(_currentState)
			_currentState->handleMessage(message);
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			setState(getInitialState());
			_currentState->enter(0);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			_timer.update(framePeriod);
			resetFrameFlags();
		}
		else if(message.getID() == MessageID::POST_LOAD)
		{
			setState(_currentStateID);
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