#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Game\StateMachineComponent.h>
#include "Pathfinder.h"
#include <vector>

namespace Temporal
{
	namespace NavigatorStates
	{
		enum Enum
		{
			WAIT,
			WALK,
			TURN,
			FALL,
			JUMP,
			DESCEND
		};


		class Wait : public ComponentState
		{
		public:
			Wait(void) {}

			virtual void handleMessage(Message& message);
		};

		class Walk : public ComponentState
		{
		public:
			Walk(void) {}

			virtual void handleMessage(Message& message);

		private:
			static const float DISTANCE_FROM_TARGET_TOLERANCE;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) {}

			virtual void handleMessage(Message& message);
		};

		class Fall : public ComponentState
		{
		public:
			Fall(void) {}

			virtual void handleMessage(Message& message);
		};

		class Jump : public ComponentState
		{
		public:
			Jump(void) {}

			virtual void handleMessage(Message& message);
		};

		class Descend : public ComponentState
		{
		public:
			Descend(void) {}

			virtual void handleMessage(Message& message);
		};
	}

	class Navigator : public StateMachineComponent
	{
	public:
		Navigator(void) : StateMachineComponent(getStates()), _destination(NULL), _path(NULL) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

		const Point& getDestination(void) const { return *_destination; }
		void setDestination(const Point* destination) { _destination = destination; }
		NavigationEdgeCollection* getPath(void) const { return _path; }
		void setPath(NavigationEdgeCollection* path) { _path = path; }
		virtual void handleMessage(Message& message);

	protected:
		virtual int getInitialState(void) const { return NavigatorStates::WAIT; }

	private:
		const Point* _destination;
		NavigationEdgeCollection* _path;

		StateCollection getStates(void) const;
	};
}
#endif