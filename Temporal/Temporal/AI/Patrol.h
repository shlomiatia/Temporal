#ifndef PATROL_H
#define PATROL_H

#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class See : public ComponentState
		{
		public:
			void enter(void) const;
			void handleMessage(Message& message) const;
		};

		class Turn : public ComponentState
		{
		public:
			void enter(void) const;
			void handleMessage(Message& message) const;
		};

		class Wait : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;

		private:
			static const float WAIT_TIME_IN_MILLIS;
		};
	}

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol(void) : StateMachineComponent(getStates(), "PAT") {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState(void) const;

	private:
		StateCollection getStates(void) const;
	};
}


#endif