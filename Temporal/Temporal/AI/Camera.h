#ifndef CAMERA_H
#define CAMERA_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace CameraStates
	{
		enum Enum
		{
			SEARCH,
			SEE,
			TURN,
			ACQUIRE
		};

		class Search : public ComponentState
		{
		public:
			void enter(void);
			void handleMessage(Message& message);

		private:
			static const float SEARCH_TIME_FOR_SIDE_IN_MILLIS;

			Timer _timer;
		};

		class See : public ComponentState
		{
		public:
			See(void) : _haveLineOfSight(false) {}

			void enter(void);
			void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) : _hasTurned(false) {}
	
			void enter(void);
			void handleMessage(Message& message);

		private:
			bool _hasTurned;
		};

		class Acquire : public ComponentState
		{
		public:
			Acquire(void) : _blinking(false), _haveLineOfSight(false) {}

			void enter(void);
			void handleMessage(Message& message);

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;
			
			Timer _timer;
			bool _blinking;
			bool _haveLineOfSight;
		};
	}

	class Camera : public StateMachineComponent
	{
	public:
		Camera(void) : StateMachineComponent(getStates()) {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		int getInitialState(void) const { return CameraStates::SEARCH; }

	private:
		StateCollection getStates(void) const;
	};
}


#endif