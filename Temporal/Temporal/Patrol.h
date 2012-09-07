#ifndef PATROL_H
#define PATROL_H

#include "StateMachineComponent.h"

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
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Turn : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Wait : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;

		private:
			static const float WAIT_TIME;
		};
	}

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol() : StateMachineComponent(getStates(), "PAT") {}

		ComponentType::Enum getType() const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState() const;

	private:
		StateCollection getStates() const;
	};
}


#endif