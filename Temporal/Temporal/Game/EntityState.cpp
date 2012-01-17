#include "EntityState.h"

namespace Temporal
{
	void EntityState::enter(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityScale(0.0f);
		stateEnter(entity);
	}

	void EntityState::exit(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::DISABLE_GRAVITY)
			entity.getBody().setGravityScale(1.0f);
		stateExit(entity);
	}

	void EntityState::update(Entity& entity)
	{
		if(_gravityResponse == GravityResponse::FALL && !(entity.getBody().getCollision() & Direction::BOTTOM))
		{
			entity.changeState(EntityStateID::FALL);
		}
		else if(_supportsHang && match(entity.getBody().getSensor(entity.HANG_SENSOR).getSensedBodyDirection(), Direction::BOTTOM | Direction::FRONT))
		{
			entity.changeState(EntityStateID::HANGING);
		}
		else
		{
			stateUpdate(entity);
		}
	}
}
