#include "StateMachineComponent.h"
#include "Message.h"
#include <Temporal\Base\Serialization.h>
#include <Temporal\Base\BaseUtils.h>

namespace Temporal
{
	StateMachineComponent::StateMachineComponent(StateCollection states, const char* prefix)
		: _states(states), _currentState(NULL), _currentStateID(Hash::INVALID), STATE_SERIALIZATION(createKey(prefix, "_STATE")), TIMER_SERIALIZATION(createKey(prefix, "_TIMER"))
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
		resetTempState();
		_timer.reset();
		_currentState->enter();
		sendMessageToOwner(Message(MessageID::STATE_ENTERED, &_currentStateID));
	}

	void StateMachineComponent::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			changeState(getInitialState());
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(STATE_SERIALIZATION, _currentStateID);
			serialization.serialize(TIMER_SERIALIZATION, _timer.getElapsedTimeInMillis());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			_timer.reset(serialization.deserializeFloat(TIMER_SERIALIZATION));
			Hash stateID = Hash(serialization.deserializeUInt(STATE_SERIALIZATION));
			
			_currentState = _states[stateID];
			_currentStateID = stateID;
			resetTempState();
		}
		// Protect against events that occur before the inital state is set TODO:
		if(_currentState != NULL)
		{
			_currentState->handleMessage(message);
		}
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(float*)message.getParam();
			resetTempState();
			_timer.update(framePeriodInMillis);
		}
	}
}