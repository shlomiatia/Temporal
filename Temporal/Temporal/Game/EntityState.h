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
		// TODO: Either pass component to entity constructor, or create a property here SLOTH!
		EntityState(EntityStateMachine& stateMachine, EntityStateGravityResponse::Enum gravityResponse, bool supportsHang, bool stopForce, const ResetAnimationParams& animation)
			: _stateMachine(stateMachine), _gravityResponse(gravityResponse), _supportsHang(supportsHang), _stopForce(stopForce), _animation(animation) {}
		virtual ~EntityState(void) {};

		virtual const char* getName(void) const = 0;
		virtual void handleMessage(Message& message);

	protected:
		EntityStateMachine& _stateMachine;

		bool isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative = Direction::NONE) const;
		bool isSensorMessage(Message& message, SensorID::Enum sensorID) const;

	private:
		const EntityStateGravityResponse::Enum _gravityResponse;
		const bool _supportsHang;
		const bool _stopForce;
		const ResetAnimationParams _animation;

		void handleGravity(bool gravityEnabled) const;

		EntityState(const EntityState&);
		EntityState& operator=(const EntityState&);
	};
}