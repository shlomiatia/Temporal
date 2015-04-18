#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include "Shapes.h"
#include "StateMachineComponent.h"
#include "Pathfinder.h"

namespace Temporal
{
	class Serialization;

	namespace NavigatorStates
	{
		class Wait : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};

		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};

		class Turn : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};

		class Fall : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};

		class JumpUp : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class JumpForward : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Descend : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};
	}

	class Navigator : public StateMachineComponent
	{
	public:
		Navigator() : StateMachineComponent(getStates(), "NAV"), _destination(OBB::Zero), _path(0) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		const OBB& getDestination() const { return _destination; }
		void setDestination(const OBB& destination) { _destination = destination; }
		NavigationEdgeList* getPath() const { return _path; }
		void setPath(NavigationEdgeList* path) { if(_path) delete _path; _path = path; }
		Component* clone() const { return new Navigator(); }

		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:

		OBB _destination;
		NavigationEdgeList* _path;

		//void deserialize(const Serialization& serialization);
		void debugDraw() const;

		HashStateMap getStates() const;
	};
}
#endif