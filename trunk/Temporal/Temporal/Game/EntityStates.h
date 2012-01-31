#pragma once

#include "EntityState.h"

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(void) 
			: EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getBody().setForce(Vector::Zero); entity.getAnimator().reset(0); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) 
			: EntityState(GravityResponse::KEEP_STATE, true) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getAnimator().reset(4); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getAnimator().reset(12, false, true); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getBody().setForce(Vector::Zero); entity.getAnimator().reset(1); }
		virtual void stateUpdate(DynamicEntity& entity);
	};


	class PrepareToJump : public EntityState
	{
	public:
		PrepareToJump(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity);

	private:
		static const float ANGLES_SIZE;
		static const float ANGLES[];
		static const EntityStateID::Type JUMP_START_STATES[];
	};

	class JumpStart : public EntityState
	{
	public:
		JumpStart(float angle, int animation, EntityStateID::Type jumpState) : EntityState(GravityResponse::FALL, false), _angle(angle), _animation(animation), _jumpState(jumpState) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getAnimator().reset(_animation); }
		virtual void stateUpdate(DynamicEntity& entity);

	private:
		float _angle;
		int _animation;
		EntityStateID::Type _jumpState;
	};

	class JumpUp : public EntityState
	{
	public:
		JumpUp(void) : EntityState(GravityResponse::KEEP_STATE, true)  {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getAnimator().reset(6); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(void) : EntityState(GravityResponse::KEEP_STATE, true)  {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getAnimator().reset(10); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(void) : EntityState(GravityResponse::FALL, false)  {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getBody().setForce(Vector::Zero); entity.getAnimator().reset(11); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false)  {};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity);
		virtual void stateUpdate(DynamicEntity& entity);

	private:
		const Rect* _platform;
	};

	class Hang : public EntityState
	{
	public:
		Hang(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getBody().setForce(Vector::Zero); entity.getAnimator().reset(7); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(GravityResponse::KEEP_STATE, false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity) { entity.getBody().setForce(Vector::Zero); entity.getAnimator().reset(2); }
		virtual void stateUpdate(DynamicEntity& entity);
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _drawCenter(Vector::Zero){};

		virtual const char* getName(void) const { return "Climbe"; }

		virtual const Vector& getDrawCenter(const DynamicEntity& entity) const { return _drawCenter; }
	protected:
		virtual void stateEnter(DynamicEntity& entity);
		virtual void stateUpdate(DynamicEntity& entity);

	private:
		Vector _drawCenter;
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : EntityState(GravityResponse::FALL, false), _platformEdge(0) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void stateEnter(DynamicEntity& entity);
		virtual void stateUpdate(DynamicEntity& entity);

	private:
		float _platformEdge;
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _drawCenter(Vector::Zero) {};

		virtual const char* getName(void) const { return "Descend"; }

		virtual const Vector& getDrawCenter(const DynamicEntity& entity) const { return _drawCenter; }
	protected:
		virtual void stateEnter(DynamicEntity& entity);
		virtual void stateUpdate(DynamicEntity& entity);

	private:
		Vector _drawCenter;
	};
}
