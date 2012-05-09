#ifndef SENTRY_H
#define SENTRY_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace SentryStates
	{
		class Search : public ComponentState
		{
		public:
			void enter(void);
			void handleMessage(Message& message);
		};

		class Acquire : public ComponentState
		{
		public:
			Acquire(void) : _haveLineOfSight(false) {}

			void enter(void);
			void handleMessage(Message& message);

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;

			bool _haveLineOfSight;
			Timer _timer;
		};

		class See : public ComponentState
		{
		public:
			See(void) : _haveLineOfSight(false) {}

			void enter(void);
			void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};
	}

	class Sentry : public StateMachineComponent
	{
	public:
		Sentry(void) : StateMachineComponent(getStates()) {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState(void) const;

	private:

		StateCollection getStates(void) const;
	};
}


#endif