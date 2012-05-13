#include "Sentry.h"
#include <Temporal\Base\Color.h>
#include <Temporal\Game\Message.h>

namespace Temporal
{
	namespace SentryStates
	{
		static const Hash SEARCH_STATE = Hash("STAT_SEN_SEARCH");
		static const Hash ACQUIRE_STATE = Hash("STAT_SEN_ACQUIRE");
		static const Hash SEE_STATE = Hash("STAT_SEN_SEE");

		void Search::enter(void) const
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Red));
		}

		void Search::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::enter(void) const
		{
			// Flag 1 - LOS
			_stateMachine->setFlag1(true);
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Yellow));
		}

		void Acquire::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getFlag1())
					_stateMachine->changeState(SEARCH_STATE);
				else if(_stateMachine->getTimer().getElapsedTimeInMillis() >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SEE_STATE);
			}
		}

		void See::enter(void) const
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Green));
		}

		void See::handleMessage(Message& message) const
		{	
			// Flag 1 - LOS
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getFlag1())
					_stateMachine->changeState(SEARCH_STATE);
			}
		}
	}

	using namespace SentryStates;

	StateCollection Sentry::getStates(void) const
	{
		StateCollection states;
		states[SEARCH_STATE] = new Search();
		states[ACQUIRE_STATE] = new Acquire();
		states[SEE_STATE] = new See();
		return states;
	}

	Hash Sentry::getInitialState(void) const
	{
		return SEARCH_STATE;
	}
}