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

		void Search::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Red));
		}

		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Yellow));
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
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);
				if(!_haveLineOfSight)
					_stateMachine->changeState(SEARCH_STATE);
				else if(_timer.getElapsedTimeInMillis() >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SEE_STATE);
				_haveLineOfSight = false;
			}
		}

		void See::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::SET_COLOR, (void*)&Color::Green));
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
					_stateMachine->changeState(SEARCH_STATE);
				_haveLineOfSight = false;
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