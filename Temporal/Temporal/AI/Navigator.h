#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Game\StateMachineComponent.h>
#include "Pathfinder.h"
#include <vector>

namespace Temporal
{
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

		const AABB& getDestination(void) const { return _destination; }
		void setDestination(const AABB& destination) { _destination = destination; }
		NavigationEdgeCollection* getPath(void) const { return _path; }
		void setPath(NavigationEdgeCollection* path) { _path = path; }
		void handleMessage(Message& message);

	protected:
		Hash getInitialState(void) const;

	private:
		AABB _destination;
		NavigationEdgeCollection* _path;

		StateCollection getStates(void) const;
	};
}
#endif