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
			virtual void enter(void* param);
			virtual void handleMessage(Message& message);

		private:
			static const float SEARCH_TIME_FOR_SIDE_IN_MILLIS;

			Timer _timer;
		};

		class See : public ComponentState
		{
		public:
			See(void) : _haveLineOfSight(false) {}

			virtual void enter(void* param);
			virtual void handleMessage(Message& message);

		private:
			bool _haveLineOfSight;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) : _hasTurned(false) {}
	
			virtual void enter(void* param);
			virtual void handleMessage(Message& message);

		private:
			bool _hasTurned;
		};

		class Acquire : public ComponentState
		{
		public:
			Acquire(void) : _blinking(false), _haveLineOfSight(false) {}

			virtual void enter(void* param);
			virtual void handleMessage(Message& message);

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

		virtual ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		virtual int getInitialState(void) const { return CameraStates::SEARCH; }

	private:
		std::vector<ComponentState*> getStates(void) const;
	};
}


#endif