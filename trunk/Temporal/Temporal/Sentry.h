#ifndef SENTRY_H
#define SENTRY_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace SentryStates
	{
		class Search : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Acquire : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;
		};

		class See : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};
	}

	class Sentry : public StateMachineComponent
	{
	public:
		Sentry() : StateMachineComponent(getStates(), "SEN") {}

		ComponentType::Enum getType() const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState() const;

	private:

		StateCollection getStates() const;
	};
}


#endif