#pragma once

namespace Temporal { class Entity; }

#include "Entity.h"

namespace Temporal
{
	namespace GravityResponse
	{
		enum Type
		{
			DISABLE_GRAVITY,
			KEEP_STATE,
			FALL
		};
	}

	class EntityState
	{
	public:
		EntityState(GravityResponse::Type gravityResponse, bool supportsHang)
			: _gravityResponse(gravityResponse), _supportsHang(supportsHang) {}
		virtual ~EntityState(void) {};

		virtual const char* getName(void) const = 0;

		virtual const Vector& getDrawCenter(const Entity& entity) const;
		void enter(Entity& entity);
		void exit(Entity& entity);
		void update(Entity& entity);

	protected:
		virtual void stateEnter(Entity& entity) {};
		virtual void stateExit(Entity& entity) {};
		virtual void stateUpdate(Entity& entity) {};

	private:
		GravityResponse::Type _gravityResponse;
		bool _supportsHang;

		EntityState(const EntityState&) {};
		EntityState& operator=(const EntityState&) {};
	};
}