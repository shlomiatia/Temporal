#ifndef DOOR_H
#define DOOR_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class Door : public StateMachineComponent
	{
	public:
		Door() : StateMachineComponent(getStates(), "DOR") {}

		Hash getType() const { return TYPE; }

		Component* clone() const { return new Door(); }

	protected:
		Hash getInitialState() const;

		static const Hash TYPE;
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