#include "Entity.h"
#include "EntityStatesFactory.h"
#include <Temporal\Input\Input.h>
#include <Temporal\Graphics\Graphics.h>
#include <sstream>

namespace Temporal
{
	const float Entity::WALK_FORCE = 5.0f;
	const float Entity::JUMP_FORCE = 20.0f;
	const float Entity::CLIMBE_FORCE = 5.0f;

	const int Entity::JUMP_SENSOR = 0;
	const int Entity::HANG_SENSOR = 1;
	const int Entity::BACK_EDGE_SENSOR = 2;
	const int Entity::FRONT_EDGE_SENSOR = 3;

	Entity::Entity(const EntityController* const controller, Body& body, const SpriteSheet& spritesheet)
		: _controller(controller), _body(body), _sprite(spritesheet), _state(EntityStatesFactory::get().getInitialState()) {}

	void Entity::changeState(EntityStateID::Type stateType)
	{
		if(_state != NULL)
		{
			_state->exit(*this);
			EntityStatesFactory::get().releaseState(_state);
		}
		_state = EntityStatesFactory::get().getState(stateType);
		//Graphics::get().setTitle(_state->getName());
		_state->enter(*this);
		_state->update(*this);
	}

	void Entity::update(void)
	{
		_state->update(*this);
		_sprite.update();
	}

	bool Entity::isMovingForward(void) const
	{
		return ((getBody().getOrientation() == Orientation::LEFT && getController().isLeft()) ||
				(getBody().getOrientation() == Orientation::RIGHT && getController().isRight()));
	}

	bool Entity::isMovingBackward(void) const
	{
		return ((getBody().getOrientation() == Orientation::RIGHT && getController().isLeft()) ||
				(getBody().getOrientation() == Orientation::LEFT && getController().isRight()));
	}
}
