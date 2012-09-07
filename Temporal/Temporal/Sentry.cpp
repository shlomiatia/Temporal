#include "Sentry.h"
#include "Color.h"

namespace Temporal
{
	namespace SentryStates
	{
		static const Hash SEARCH_STATE = Hash("SEN_STT_SEARCH");
		static const Hash ACQUIRE_STATE = Hash("SEN_STT_ACQUIRE");
		static const Hash SEE_STATE = Hash("SEN_STT_SEE");

		void Search::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::SET_COLOR, const_cast<Color*>(&Color::Red)));
		}

		void Search::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
		}

		const float Acquire::ACQUIRE_TIME(1.0f);

		void Acquire::enter() const
		{
			// TempFlag 1 - LOS
			_stateMachine->setTempFlag1(true);
			_stateMachine->raiseMessage(Message(MessageID::SET_COLOR, const_cast<Color*>(&Color::Yellow)));
		}

		void Acquire::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setTempFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getTempFlag1())
					_stateMachine->changeState(SEARCH_STATE);
				else if(_stateMachine->getTimer().getElapsedTime() >= ACQUIRE_TIME)
					_stateMachine->changeState(SEE_STATE);
			}
		}

		void See::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::SET_COLOR, const_cast<Color*>(&Color::Green)));
		}

		void See::handleMessage(Message& message) const
		{	
			// TempFlag 1 - LOS
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setTempFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getTempFlag1())
					_stateMachine->changeState(SEARCH_STATE);
			}
		}
	}

	using namespace SentryStates;

	StateCollection Sentry::getStates() const
	{
		StateCollection states;
		states[SEARCH_STATE] = new Search();
		states[ACQUIRE_STATE] = new Acquire();
		states[SEE_STATE] = new See();
		return states;
	}

	Hash Sentry::getInitialState() const
	{
		return SEARCH_STATE;
	}
}