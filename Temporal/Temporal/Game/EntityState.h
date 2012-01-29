#pragma once

namespace Temporal { class DynamicEntity; }

#include "DynamicEntity.h"

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
		// TODO: Accept animation, movements
		EntityState(GravityResponse::Type gravityResponse, bool supportsHang)
			: _gravityResponse(gravityResponse), _supportsHang(supportsHang) {}
		virtual ~EntityState(void) {};

		virtual const char* getName(void) const = 0;

		virtual const Vector& getDrawCenter(const DynamicEntity& entity) const;
		void enter(DynamicEntity& entity);
		void exit(DynamicEntity& entity);
		void update(DynamicEntity& entity);

	protected:
		virtual void stateEnter(DynamicEntity& entity) {};
		virtual void stateExit(DynamicEntity& entity) {};
		virtual void stateUpdate(DynamicEntity& entity) {};

	private:
		GravityResponse::Type _gravityResponse;
		bool _supportsHang;

		EntityState(const EntityState&) {};
		EntityState& operator=(const EntityState&) {};
	};
}