#include "EntityState.h"

namespace Temporal
{
	const Vector& EntityState::getDrawCenter(const DynamicEntity& entity) const
	{
		return entity.getBody().getBounds().getCenter(); 
	}

	void EntityState::enter(DynamicEntity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityEnabled(false);
		stateEnter(entity);
	}

	void EntityState::exit(DynamicEntity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityEnabled(true);
		stateExit(entity);
	}

	void EntityState::update(DynamicEntity& entity)
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
