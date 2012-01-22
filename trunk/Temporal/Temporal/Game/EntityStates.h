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

		virtual void stateUpdate(Entity& entity);
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) 
			: EntityState(GravityResponse::KEEP_STATE, true) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void stateUpdate(Entity& entity);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Walk"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "Turn"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		// TODO: Delete this shit
		int _crap;
	};

	class Jump : public EntityState
	{
	public:
		Jump(void) : EntityState(GravityResponse::KEEP_STATE, true), _force(Vector::Zero) {};

		virtual const char* getName(void) const { return "Jump"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		static const float ANGLES_SIZE;
		static const float ANGLES[];
		Vector _force;
		bool _isJumpStarted;

	};

	class Hanging : public EntityState
	{
	public:
		Hanging(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false)  {};

		virtual const char* getName(void) const { return "Hanging"; }

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

		virtual void stateUpdate(Entity& entity);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(GravityResponse::KEEP_STATE, false) {};

		virtual const char* getName(void) const { return "Drop"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _crap(0) {};

		virtual const char* getName(void) const { return "Climbe"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		// TODO: Remove this shit
		int _crap;
	};

	class PrepareToDescend : public EntityState
	{
	public:
		PrepareToDescend(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		float _platformEdge;
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _crap(0) {};

		virtual const char* getName(void) const { return "Descend"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		// TODO: Remove this shit
		int _crap;
	};
}
