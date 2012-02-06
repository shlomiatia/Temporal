#pragma once

#include "EntityState.h"
#include <Temporal/Physics/Body.h>

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::STAND)) ,_isDescending(false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public EntityState
	{
	public:
		Fall(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::FALL)) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Walk : public EntityState
	{
	public:
		Walk(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(AnimationID::WALK, false, true)), _stillWalking(false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public EntityState
	{
	public:
		Turn(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::TURN)) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void handleMessage(Message& message);
	};


	class PrepareToJump : public EntityState
	{
	public:
		PrepareToJump(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::STAND)) {};

		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		static const float ANGLES_SIZE;
		static const float ANGLES[];
		static const EntityStateID::Enum JUMP_START_STATES[];

		EntityStateID::Enum _jumpStartState;

		void handleJumpSensor(Message &message);
	};

	class JumpStart : public EntityState
	{
	public:
		JumpStart(EntityStateMachine& stateMachine, float angle, AnimationID::Enum animation, EntityStateID::Enum jumpState) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(animation)), _angle(angle), _jumpState(jumpState), _platformFound(false) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		float _angle;
		EntityStateID::Enum _jumpState;
		bool _platformFound;
	};

	class JumpUp : public EntityState
	{
	public:
		JumpUp(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::JUMP_UP))  {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::JUMP_FORWARD))  {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::JUMP_FORWARD_END))  {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::HANG_FORWARD, true)), _platform(NULL) {};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _person;
		const Body* _platform;

		void update(void);
	};

	class Hang : public EntityState
	{
	public:
		Hang(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::HANG)) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Drop : public EntityState
	{
	public:
		Drop(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::KEEP_STATE, false, true, ResetAnimationParams(AnimationID::DROP)), _platformFound(false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::DISABLE_GRAVITY, false, false, ResetAnimationParams(AnimationID::CLIMBE)), _moved(false) {};

		virtual const char* getName(void) const { return "Climbe"; }
	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _moved;
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(AnimationID::STAND)) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _person;
		const Body* _platform;

		void update(void);
	};

	class Descend : public EntityState
	{
	public:
		Descend(EntityStateMachine& stateMachine) : EntityState(stateMachine, EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::CLIMBE, true)){};

		virtual const char* getName(void) const { return "Descend"; }
	protected:
		virtual void handleMessage(Message& message);
	};
}
