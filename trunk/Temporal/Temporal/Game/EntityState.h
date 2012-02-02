#pragma once

namespace Temporal { class EntityStateMachine; }

#include "Message.h"
#include "EntityStateMachine.h"
#include "MessageParams.h"

namespace Temporal
{
	class EntityState
	{
	public:
		EntityState(EntityStateGravityResponse::Type gravityResponse, bool supportsHang, bool stopForce, const ResetAnimationParams& animation)
			: _gravityResponse(gravityResponse), _supportsHang(supportsHang), _stopForce(stopForce), _animation(animation) {}
		virtual ~EntityState(void) {};

		virtual const char* getName(void) const = 0;

		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	protected:
		void sendMessage(EntityStateMachine& stateMachine, Message& message);
		bool isSensorMessage(Message& message, SensorID::Type sensorID);

	private:
		const EntityStateGravityResponse::Type _gravityResponse;
		const bool _supportsHang;
		const bool _stopForce;
		const ResetAnimationParams _animation;

		void handleGravity(EntityStateMachine& stateMachine, bool gravityEnabled);

		EntityState(const EntityState&);
		EntityState& operator=(const EntityState&);
	};
}