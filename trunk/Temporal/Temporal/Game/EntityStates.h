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
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector(0.0f, 0.0f)); entity.getSprite().reset(0); }
		virtual void stateUpdate(Entity& entity);
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) 
			: EntityState(GravityResponse::KEEP_STATE, true) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getSprite().reset(4); }
		virtual void stateUpdate(Entity& entity);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector(5.0f, 0.0f)); entity.getSprite().reset(12, false, true); }
		virtual void stateUpdate(Entity& entity);
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector::Zero); entity.getSprite().reset(1); }
		virtual void stateUpdate(Entity& entity);
	};


	// TODO: Redirect to actual jump start classes
	class JumpStart : public EntityState
	{
	public:
		JumpStart(void) : EntityState(GravityResponse::FALL, false), _force(Vector::Zero) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		static const float ANGLES_SIZE;
		static const float ANGLES[];
		Vector _force;
		bool _isJumpingForward;
	};

	class JumpUp : public EntityState
	{
	public:
		JumpUp(void) : EntityState(GravityResponse::KEEP_STATE, true)  {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getSprite().reset(6); }
		virtual void stateUpdate(Entity& entity);
	};

	class JumpForward : public EntityState
	{
	public:
		JumpForward(void) : EntityState(GravityResponse::KEEP_STATE, true)  {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getSprite().reset(10); }
		virtual void stateUpdate(Entity& entity);
	};

	class JumpForwardEnd : public EntityState
	{
	public:
		JumpForwardEnd(void) : EntityState(GravityResponse::FALL, false)  {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector::Zero); entity.getSprite().reset(11); }
		virtual void stateUpdate(Entity& entity);
	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false)  {};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		const Rect* _platform;
	};

	class Hang : public EntityState
	{
	public:
		Hang(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector::Zero); entity.getSprite().reset(7); }
		virtual void stateUpdate(Entity& entity);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(GravityResponse::KEEP_STATE, false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void stateEnter(Entity& entity) { entity.getBody().setForce(Vector::Zero); entity.getSprite().reset(2); }
		virtual void stateUpdate(Entity& entity);
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _drawCenter(Vector::Zero), _isFinished(false) {};

		virtual const char* getName(void) const { return "Climbe"; }

		virtual const Vector& getDrawCenter(const Entity& entity) const { return _drawCenter; }
	protected:
		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		Vector _drawCenter;
		bool _isFinished;
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : EntityState(GravityResponse::FALL, false), _platformEdge(0) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		float _platformEdge;
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _drawCenter(Vector::Zero), _isFinished(0) {};

		virtual const char* getName(void) const { return "Descend"; }

		virtual const Vector& getDrawCenter(const Entity& entity) const { return _drawCenter; }
	protected:
		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		Vector _drawCenter;
		bool _isFinished;
	};
}
