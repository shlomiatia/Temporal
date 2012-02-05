#pragma once

#include "EntityState.h"
#include <Temporal/Physics/Body.h>

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(void) 
			: EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(0)) ,_isDescending(false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) 
			: EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(4)) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(12, false, true)), _stillWalking(false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(1)) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};


	class PrepareToJump : public EntityState
	{
	public:
		PrepareToJump(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(0)), _platformFound(false) {};

		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		static const float ANGLES_SIZE;
		static const float ANGLES[];
		static const EntityStateID::Type JUMP_START_STATES[];

		EntityStateID::Type _jumpStartState;
		bool _platformFound;
	};

	class JumpStart : public EntityState
	{
	public:
		JumpStart(float angle, int animation, EntityStateID::Type jumpState) : EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(animation)), _angle(angle), _jumpState(jumpState), _platformFound(false) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		float _angle;
		EntityStateID::Type _jumpState;
		bool _platformFound;
	};

	class JumpUp : public EntityState
	{
	public:
		JumpUp(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(6))  {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
		void update(EntityStateMachine& stateMachine);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, true, false, ResetAnimationParams(10))  {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(void) : EntityState(EntityStateGravityResponse::FALL, false, true, ResetAnimationParams(11))  {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(14, true)), _platform(NULL) {};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		const Body* _person;
		const Body* _platform;
	};

	class Hang : public EntityState
	{
	public:
		Hang(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(7)) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(EntityStateGravityResponse::KEEP_STATE, false, true, ResetAnimationParams(2)), _platformFound(false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		bool _platformFound;
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, false, ResetAnimationParams(8)) {};

		virtual const char* getName(void) const { return "Climbe"; }
	protected:
		void enter(EntityStateMachine& stateMachine);
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : EntityState(EntityStateGravityResponse::FALL, false, false, ResetAnimationParams(0)) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);

	private:
		const Body* _person;
		const Body* _platform;
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) : EntityState(EntityStateGravityResponse::DISABLE_GRAVITY, false, true, ResetAnimationParams(8, true)){};

		virtual const char* getName(void) const { return "Descend"; }
	protected:
		void enter(EntityStateMachine& stateMachine);
		virtual void handleMessage(EntityStateMachine& stateMachine, Message& message);
	};
}
