#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include "AABB.h"
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
			void enter(void) const;
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
		Navigator(void) : StateMachineComponent(getStates(), "NAV"), _destination(AABB::Zero), _path(NULL) {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }
		void handleMessage(Message& message);

		const AABB& getDestination(void) const { return _destination; }
		void setDestination(const AABB& destination) { _destination = destination; }
		NavigationEdgeCollection* getPath(void) const { return _path; }
		void setPath(NavigationEdgeCollection* path) { if(_path != NULL) delete _path; _path = path; }

	protected:
		Hash getInitialState(void) const;

	private:
		AABB _destination;
		NavigationEdgeCollection* _path;

		void deserialize(const Serialization& serialization);
		void debugDraw(void) const;

		StateCollection getStates(void) const;
	};
}
#endif