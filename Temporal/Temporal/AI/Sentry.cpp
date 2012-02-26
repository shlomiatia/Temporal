#include "Sentry.h"
#include <Temporal/Game/MessageParams.h>
#include <Temporal/Game/World.h>
#include <Temporal/Graphics/Graphics.h>

namespace Temporal
{
	bool SentryState::haveLineOfSight(void) const
	{
		const Vector& sentryPosition = *(const Vector* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sentryOrientation = *(const Orientation::Enum* const)_stateMachine->sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Vector& targetPosition = *(const Vector* const)World::get().sendQueryMessageToEntity(0, Message(MessageID::GET_POSITION));
		if((targetPosition.getX() - sentryPosition.getX()) * sentryOrientation < 0.0f)
			return false;
		RayCastParams params(targetPosition);
		Message message(MessageID::RAY_CAST, &params);
		_stateMachine->sendMessageToOwner(message);
		return params.getResult();
	}

	void Search::handleMessage(Message& message)
	{	
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Red));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(haveLineOfSight())
				_stateMachine->changeState(SentryStates::ACQUIRE);
		}
	}

	const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

	void Acquire::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Yellow));
			_time = 0.0f;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *(const float* const)message.getParam();
			_time += framePeriodInMillis;
			if(!haveLineOfSight())
				_stateMachine->changeState(SentryStates::SEARCH);
			else if(_time >= ACQUIRE_TIME_IN_MILLIS)
				_stateMachine->changeState(SentryStates::SEE);
		}
	}

	void See::handleMessage(Message& message)
	{	
		if(message.getID() == MessageID::ENTER_STATE)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Green));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(!haveLineOfSight())
				_stateMachine->changeState(SentryStates::SEARCH);
		}
	}

	void Sentry::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);

		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			/*const Vector& sentryPosition = *(const Vector* const)sendQueryMessageToOwner(Message(MessageID::GET_POSITION));
			const Vector& targetPosition = *(const Vector* const)World::get().sendQueryMessageToEntity(0, Message(MessageID::GET_POSITION));

			RayCastParams params(targetPosition);
			Message message(MessageID::RAY_CAST, &params);
			sendMessageToOwner(message);
			//Graphics::get().drawLine(sentryPosition, targetPosition);*/
		}
	}

	std::vector<ComponentState*> Sentry::getStates() const
	{
		std::vector<ComponentState*> states;
		states.push_back(new Search());
		states.push_back(new Acquire());
		states.push_back(new See());
		return states;
	}

}