#pragma once

#include <Temporal/Game/StateMachineComponent.h>

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

			virtual void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) {}

			virtual void handleMessage(Message& message);
		};

		class Wait : public ComponentState
		{
		public:
			Wait(void) : _elapsedTimeInMillis(0.0f) {}

			virtual void handleMessage(Message& message);

		private:
			static const float WAIT_TIME_IN_MILLIS;

			float _elapsedTimeInMillis;
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
		std::vector<ComponentState*> getStates() const;
	};
}

