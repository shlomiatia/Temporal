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
		static const Hash TYPE;

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
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Close : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Opening : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Closing : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif