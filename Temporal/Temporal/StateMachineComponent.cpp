#include "StateMachineComponent.h"
#include "Serialization.h"
#include "BaseUtils.h"

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(StateCollection states, const char* prefix)
		: _states(states), _currentState(NULL), _currentStateID(Hash::INVALID), STATE_SERIALIZATION(createKey(prefix, "_STATE")), TIMER_SERIALIZATION(createKey(prefix, "_TIMER"))
	{
		for(StateIterator i = _states.begin(); i != _states.end(); ++i)
			(*(*i).second).setStateMachine(this);
	}

	StateMachineComponent::~StateMachineComponent()
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
		setState(stateID);
		_currentState->enter();
		sendMessageToOwner(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			setState(getInitialState());
			_currentState->enter();
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			serialization.serialize(STATE_SERIALIZATION, _currentStateID);
			serialization.serialize(TIMER_SERIALIZATION, _timer.getElapsedTimeInMillis());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			_timer.reset(serialization.deserializeFloat(TIMER_SERIALIZATION));
			Hash stateID = Hash(serialization.deserializeUInt(STATE_SERIALIZATION));
			setState(stateID);
		}
		_currentState->handleMessage(message);
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *static_cast<float*>(message.getParam());
			_timer.update(framePeriodInMillis);
			resetTempState();
		}
	}

	void StateMachineComponent::setState(const Hash& stateID)
	{
		_currentState = _states[stateID];
		_currentStateID = stateID;
		resetTempState();
		_timer.reset();
	}
}