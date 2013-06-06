#ifndef DOOR_H
#define DOOR_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class Door : public StateMachineComponent
	{
	public:
		Door() : StateMachineComponent(getStates(), "DOR") {}

		ComponentType::Enum getType() const { return ComponentType::OTHER; }

		Component* clone() const { return new Door(); }

	protected:
		Hash getInitialState() const;

	private:
		StateCollection getStates() const;
	};

	namespace DoorStates
	{
		class Open : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Close : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Opening : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Closing : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};
	}
}

#endif