#pragma once

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

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

		class Search : public ComponentState
		{
		public:
			virtual void enter(const void* param);
			virtual void handleMessage(Message& message);
		};

		class Acquire : public ComponentState
		{
		public:
			Acquire(void) : _haveLineOfSight(false) {}

			virtual void enter(const void* param);
			virtual void handleMessage(Message& message);

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;

			bool _haveLineOfSight;
			Timer _timer;
		};

		class See : public ComponentState
		{
		public:
			See(void) : _haveLineOfSight(false) {}

			virtual void enter(const void* param);
			virtual void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};
	}

	class Sentry : public StateMachineComponent
	{
	public:
		Sentry(void) : StateMachineComponent(getStates()) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		virtual int getInitialState(void) const { return SentryStates::SEARCH; }

	private:

		std::vector<ComponentState*> getStates(void) const;
	};
}

