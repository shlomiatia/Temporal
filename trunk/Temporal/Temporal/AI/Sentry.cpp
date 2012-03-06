#include "Sentry.h"
#include <Temporal/Game/MessageParams.h>
#include <Temporal/Game/EntitiesManager.h>
#include <Temporal/Graphics/Graphics.h>

namespace Temporal
{
	namespace SentryStates
	{
		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Red));
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(SentryStates::ACQUIRE);
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Yellow));
				_elapsedTimeInMillis = 0.0f;
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(const float* const)message.getParam();
				_elapsedTimeInMillis += framePeriodInMillis;
				if(!_haveLineOfSight)
					_stateMachine->changeState(SentryStates::SEARCH);
				else if(_elapsedTimeInMillis >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SentryStates::SEE);
				_haveLineOfSight = false;
			}
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::ENTER_STATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Green));
				_haveLineOfSight = false;
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_haveLineOfSight)
					_stateMachine->changeState(SentryStates::SEARCH);
				_haveLineOfSight = false;
			}
		}
	}

	std::vector<ComponentState*> Sentry::getStates() const
	{
		std::vector<ComponentState*> states;
		using namespace SentryStates;
		states.push_back(new Search());
		states.push_back(new Acquire());
		states.push_back(new See());
		return states;
	}

}