#pragma once
#include <Temporal\Base\Vector.h>
#include <Temporal\Game\StateMachineComponent.h>
#include "Pathfinder.h"
#include <vector>

namespace Temporal
{
	// TODO: Fault tolerance, jump without hang, drop, fix huristics so it will take into account several edges AI (turn, position)
	namespace NavigatorStates
	{
		enum Enum
		{
			WAIT,
			WALK,
			TURN,
			FALL,
			JUMP
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
	}

	class Navigator : public StateMachineComponent
	{
	public:
		Navigator(void) : StateMachineComponent(getStates()), _destination(NULL), _path(NULL) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

		const Vector& getDestination(void) const { return *_destination; }
		void setDestination(const Vector* destination) { _destination = destination; }
		std::vector<const NavigationEdge* const>* getPath(void) const { return _path; }
		void setPath(std::vector<const NavigationEdge* const>* path) { _path = path; }
		virtual void handleMessage(Message& message);

	protected:
		virtual int getInitialState(void) const { return NavigatorStates::WAIT; }

	private:
		const Vector* _destination;
		std::vector<const NavigationEdge* const>* _path;

		std::vector<ComponentState*> getStates(void) const;
	};
}