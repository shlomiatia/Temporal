#pragma once

#include "AIStateMachine.h"
#include "AIState.h"

namespace Temporal
{
	namespace SentryStates
	{
		enum Enum
		{
			SEARCH,
			ACQUIRE,
			SEE
		};
	}

	class SentryState : public AIState
	{
	public:
		virtual ~SentryState(void) {};

	protected:
		bool haveLineOfSight(void) const;
	};

	class Search : public SentryState
	{
	public:
		virtual void handleMessage(Message& message);
	};

	class Acquire : public SentryState
	{
	public:
		Acquire(void) : _time(0.0f) {}

		virtual void handleMessage(Message& message);

	private:
		static const float ACQUIRE_TIME_IN_MILLIS;

		float _time;
	};

	class See : public SentryState
	{
	public:
		virtual void handleMessage(Message& message);
	};

	class Sentry : public AIStateMachine
	{
	public:

		std::vector<AIState*> getStates() const
		{
			std::vector<AIState*> states;
			states.push_back(new Search());
			states.push_back(new Acquire());
			states.push_back(new See());
			return states;
		}

		Sentry(void) : AIStateMachine(getStates(), SentryStates::SEARCH) {}
	};
}

