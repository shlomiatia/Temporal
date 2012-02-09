#pragma once

#include "EntityState.h"
#include <Temporal/Physics/Body.h>

// TODO: Order the mother fuckers states SLOTH!

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::STAND)) ,_isDescending(false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::FALL)) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(AnimationID::WALK, false, true)), _stillWalking(false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::TURN)) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void handleMessage(Message& message);
	};


	class PrepareToJump : public EntityState
	{
	public:
		PrepareToJump(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::STAND)) {};

		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		static const EntityStateID::Enum JUMP_ANGLES_START_STATES[];

		EntityStateID::Enum _jumpStartState;

		void handleJumpSensor(Message &message);
	};

	class JumpStart : public EntityState
	{
	public:
		JumpStart(float angle, AnimationID::Enum animation, EntityStateID::Enum jumpState) : 
		  EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(animation)), _angle(angle), _jumpState(jumpState), _platformFound(false) {};

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
		JumpUp(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::JUMP_UP))  {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(AnimationID::JUMP_FORWARD))  {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(AnimationID::JUMP_FORWARD_END))  {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::HANG_FORWARD, true)), _person(NULL), _platform(NULL) {};

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
		Hang(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::HANG)) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, false, true, ResetAnimationParams(AnimationID::DROP)), _platformFound(false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climb : public EntityState
	{
	public:
		Climb(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, false, ResetAnimationParams(AnimationID::CLIMB)) {};

		virtual const char* getName(void) const { return "Climb"; }
	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(AnimationID::STAND)), _person(NULL), _platform(NULL) {};

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
		Descend(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(AnimationID::CLIMB, true)){};

		virtual const char* getName(void) const { return "Descend"; }
	protected:
		virtual void handleMessage(Message& message);
	};
}
