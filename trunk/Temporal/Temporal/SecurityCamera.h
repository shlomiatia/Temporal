#ifndef SECURITYCAMERA_H
#define SECURITYCAMERA_H

#include "StateMachineComponent.h"

namespace Temporal
{
	namespace SecurityCameraStates
	{
		class Search : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);

		private:
			static const float SEARCH_TIME;
		};

		class See : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Turn : public ComponentState
		{
		public:
			Turn() : _hasTurned(false) {}

			void enter(void* param);
			void handleMessage(Message& message);

			mutable bool _hasTurned;

		};

		class Acquire : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);

		private:
			static const float ACQUIRE_TIME;

			void update() const;
		};
	}

	class SecurityCamera : public StateMachineComponent
	{
	public:
		SecurityCamera() : StateMachineComponent(getStates(), "CAM") {}

		Hash getType() const { return TYPE; }
		Component* clone() const { return new SecurityCamera(); }

		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:

		StateCollection getStates() const;
	};
}


#endif