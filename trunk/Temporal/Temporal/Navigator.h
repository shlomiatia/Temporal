#ifndef NAVIGATOR_H
#define NAVIGATOR_H
/*#include "Shapes.h"
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
			void handleMessage(Message& message) const;
		};

		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class Turn : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class Fall : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class JumpUp : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class JumpForward : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Descend : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
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
		NavigationEdgeCollection* getPath() const { return _path; }
		void setPath(NavigationEdgeCollection* path) { if(_path) delete _path; _path = path; }
		Component* clone() const { return new Navigator(); }

	protected:
		Hash getInitialState() const;

		static const Hash TYPE;
	private:

		OBB _destination;
		NavigationEdgeCollection* _path;

		//void deserialize(const Serialization& serialization);
		void debugDraw() const;

		StateCollection getStates() const;
	};
}*/
#endif