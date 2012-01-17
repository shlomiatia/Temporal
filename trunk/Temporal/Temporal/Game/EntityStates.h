#pragma once

#include "EntityState.h"

namespace Temporal
{
	class Stand : public EntityState
	{
	public:
		Stand(void) 
			: EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) { return "Stand"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Fall : public EntityState
	{
	public:
		Fall(void) 
			: EntityState(GravityResponse::KEEP_STATE, true) {};

		virtual const char* getName(void) { return "Fall"; }

	protected:
		virtual void stateUpdate(Entity& entity);
	};

	class Walk : public EntityState
	{
	public:
		Walk(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) { return "Walk"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Turn : public EntityState
	{
	public:
		Turn(void) : EntityState(GravityResponse::FALL, false) {};

		virtual const char* getName(void) { return "Turn"; }

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

		virtual const char* getName(void) { return "Jump"; }

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

		virtual const char* getName(void) { return "Hanging"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		const Rect* _platform;

	};

	class Hang : public EntityState
	{
	public:
		Hang(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false) {};

		virtual const char* getName(void) { return "Hang"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Drop : public EntityState
	{
	public:
		Drop(void) : EntityState(GravityResponse::KEEP_STATE, false) {};

		virtual const char* getName(void) { return "Drop"; }

		virtual void stateUpdate(Entity& entity);
	};

	class Climbe : public EntityState
	{
	public:
		Climbe(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _crap(0) {};

		virtual const char* getName(void) { return "Climbe"; }

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

		virtual const char* getName(void) { return "PrepareToDescend"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		float _platformEdge;
	};

	class Descend : public EntityState
	{
	public:
		Descend(void) : EntityState(GravityResponse::DISABLE_GRAVITY, false), _crap(0) {};

		virtual const char* getName(void) { return "Descend"; }

		virtual void stateEnter(Entity& entity);
		virtual void stateUpdate(Entity& entity);

	private:
		// TODO: Remove this shit
		int _crap;
	};
}
