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
			Wait(void) {}

			void handleMessage(Message& message);
		};

		class Walk : public ComponentState
		{
		public:
			Walk(void) {}

			void handleMessage(Message& message);
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) {}

			void handleMessage(Message& message);
		};

		class Fall : public ComponentState
		{
		public:
			Fall(void) {}

			void handleMessage(Message& message);
		};

		class JumpUp : public ComponentState
		{
		public:
			JumpUp(void) {}

			void handleMessage(Message& message);
		};

		class JumpForward : public ComponentState
		{
		public:
			JumpForward(void) {}

			void enter(void);
			void handleMessage(Message& message);
		};

		class Descend : public ComponentState
		{
		public:
			Descend(void) {}

			void handleMessage(Message& message);
		};
	}

	class Navigator : public StateMachineComponent
	{
	public:
		Navigator(void) : StateMachineComponent(getStates()), _destination(AABB::Empty), _path(NULL) {}

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