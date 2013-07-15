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
			static const float ACQUIRE_TIME;
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

		Hash getType() const { return TYPE; }
		Component* clone() const { return new Sentry(); }

		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:

		StateCollection getStates() const;
	};
}


#endif