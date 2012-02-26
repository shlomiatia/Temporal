#pragma once

#include <Temporal/Game/StateMachineComponent.h>

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

	class SentryState : public ComponentState
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

	class Sentry : public StateMachineComponent
	{
	public:
		Sentry(void) : StateMachineComponent(getStates()) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }
		virtual void handleMessage(Message& message);

	protected:
		virtual int getInitialState(void) const { return SentryStates::SEARCH; }

	private:
		std::vector<ComponentState*> getStates() const;
	};
}

