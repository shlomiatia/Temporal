#include "Sentry.h"
#include <Temporal\Base\Color.h>
#include <Temporal\Game\Message.h>

namespace Temporal
{
	namespace SentryStates
	{
		void Search::enter(const void* param)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Red));
		}

		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(SentryStates::ACQUIRE);
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::enter(const void* param)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Yellow));
			_timer.reset();
			_haveLineOfSight = true;
		}

		void Acquire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(const float* const)message.getParam();
				_timer.update(framePeriodInMillis);
				if(!_haveLineOfSight)
					_stateMachine->changeState(SentryStates::SEARCH);
				else if(_timer.getElapsedTimeInMillis() >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SentryStates::SEE);
				_haveLineOfSight = false;
			}
		}

		void See::enter(const void* param)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, &Color::Green));
			_haveLineOfSight = false;
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
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

	std::vector<ComponentState*> Sentry::getStates(void) const
	{
		std::vector<ComponentState*> states;
		using namespace SentryStates;
		states.push_back(new Search());
		states.push_back(new Acquire());
		states.push_back(new See());
		return states;
	}

}