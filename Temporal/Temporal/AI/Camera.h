#ifndef CAMERA_H
#define CAMERA_H

#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	namespace CameraStates
	{
		class Search : public ComponentState
		{
		public:
			void enter(void) const;
			void handleMessage(Message& message) const;

		private:
			static const float SEARCH_TIME_FOR_SIDE_IN_MILLIS;
		};

		class See : public ComponentState
		{
		public:
			void enter(void) const;
			void handleMessage(Message& message) const;
		};

		class Turn : public ComponentState
		{
		public:
			Turn(void) : _hasTurned(false) {}

			void enter(void) const;
			void handleMessage(Message& message) const;

			mutable bool _hasTurned;

		};

		class Acquire : public ComponentState
		{
		public:
			void enter(void) const;
			void handleMessage(Message& message) const;

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;
			static const float BLINK_TIME_IN_MILLIS;
		};
	}

	class Camera : public StateMachineComponent
	{
	public:
		Camera(void) : StateMachineComponent(getStates(), "CAM") {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState(void) const;

	private:
		StateCollection getStates(void) const;
	};
}


#endif