#ifndef PATROL_H
#define PATROL_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace PatrolStates
	{
		enum Enum
		{
			WALK,
			SEE,
			TURN,
			WAIT
		};


		class Walk : public ComponentState
		{
		public:
			Walk(void) {}

			virtual void handleMessage(Message& message);
		};

		class See : public ComponentState
		{
		public:
			See(void) : _haveLineOfSight(false) {}

			virtual void enter(void);
			virtual void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) {}

			virtual void enter(void);
			virtual void handleMessage(Message& message);
		};

		class Wait : public ComponentState
		{
		public:
			virtual void enter(void);
			virtual void handleMessage(Message& message);

		private:
			static const float WAIT_TIME_IN_MILLIS;

			Timer _timer;
		};
	}

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol(void) : StateMachineComponent(getStates()) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		virtual int getInitialState(void) const { return PatrolStates::WALK; }

	private:
		StateCollection getStates(void) const;
	};
}


#endif