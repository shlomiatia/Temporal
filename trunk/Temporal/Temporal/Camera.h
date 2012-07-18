#ifndef CAMERA_H
#define CAMERA_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace CameraStates
	{
		class Search : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;

		private:
			static const float SEARCH_TIME_FOR_SIDE_IN_MILLIS;
		};

		class See : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Turn : public ComponentState
		{
		public:
			Turn() : _hasTurned(false) {}

			void enter() const;
			void handleMessage(Message& message) const;

			mutable bool _hasTurned;

		};

		class Acquire : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;

		private:
			static const float ACQUIRE_TIME_IN_MILLIS;
			static const float BLINK_TIME_IN_MILLIS;

			void update() const;
		};
	}

	class Camera : public StateMachineComponent
	{
	public:
		Camera() : StateMachineComponent(getStates(), "CAM") {}

		ComponentType::Enum getType() const { return ComponentType::AI_CONTROLLER; }

	protected:
		Hash getInitialState() const;

	private:
		StateCollection getStates() const;
	};
}


#endif