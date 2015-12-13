#ifndef SECURITYCAMERA_H
#define SECURITYCAMERA_H

#include "StateMachineComponent.h"

namespace Temporal
{
	class RayCastResult;

	namespace SecurityCameraStates
	{
		class Search : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class See : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		private:
			bool samePeriod(Entity& entity);
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
		void handleMessage(Message& message);

		void trackTarget(RayCastResult& result);

		static const Hash TYPE;
		static const Hash ALARM_TARGET_ID;
	protected:
		Hash getInitialState() const;

	private:
		HashStateMap getStates() const;

		friend class SerializationAccess;
	};
}


#endif