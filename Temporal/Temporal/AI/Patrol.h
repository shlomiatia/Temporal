#ifndef PATROL_H
#define PATROL_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			Walk(void) {}

			void handleMessage(Message& message);
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

		class Turn : public ComponentState
		{
		public:
			Turn(void) {}

			void enter(void);
			void handleMessage(Message& message);
		};

		class Wait : public ComponentState
		{
		public:
			void enter(void);
			void handleMessage(Message& message);

		private:
			static const float WAIT_TIME_IN_MILLIS;

			Timer _timer;
		};
	}

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol(void) : StateMachineComponent(getStates()) {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState(void) const;

	private:
		StateCollection getStates(void) const;
	};
}


#endif