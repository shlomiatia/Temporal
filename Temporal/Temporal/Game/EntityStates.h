#pragma once

#include "EntityState.h"
#include <Temporal/Physics/Body.h>

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(void) : _isDescending(false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : _stillWalking(false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToJump : public EntityState
	{
	public:
		PrepareToJump(void) {};

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
		  _angle(angle), _animation(animation), _jumpState(jumpState), _platformFound(false), _animationEnded(false) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		float _angle;
		AnimationID::Enum _animation;
		EntityStateID::Enum _jumpState;
		bool _platformFound;
		bool _animationEnded;
	};

	class JumpUp : public EntityState
	{
	public:
		JumpUp(void) {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(void) {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(void) {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToHang : public EntityState
	{
	public:
		PrepareToHang(void) : _platform(NULL) {};

		virtual const char* getName(void) const { return "PrepareToHang"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _platform;

		void update(void);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void){};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Hang : public EntityState
	{
	public:
		Hang(void) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : _platformFound(false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climb : public EntityState
	{
	public:
		Climb(void) {};

		virtual const char* getName(void) const { return "Climb"; }
	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : _platform(NULL) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _platform;

		void update(void);
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) {};

		virtual const char* getName(void) const { return "Descend"; }
	protected:
		virtual void handleMessage(Message& message);
	};
}
