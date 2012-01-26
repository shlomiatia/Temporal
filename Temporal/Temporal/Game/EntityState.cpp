#include "EntityState.h"

namespace Temporal
{
	const Vector& EntityState::getDrawCenter(const Entity& entity) const
	{
		return entity.getBody().getBounds().getCenter(); 
	}

	void EntityState::enter(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityEnabled(false);
		stateEnter(entity);
	}

	void EntityState::exit(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityEnabled(true);
		stateExit(entity);
	}

	void EntityState::update(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::FALL && !(entity.getBody().getCollision() & Direction::BOTTOM))
		{
			entity.changeState(EntityStateID::FALL);
		}
		else if(_supportsHang && entity.getBody().getSensor(entity.HANG_SENSOR).isSensing())
		{
			entity.changeState(EntityStateID::HANGING);
		}
		else
		{
			stateUpdate(entity);
		}
	}
}
