#include "DynamicEntity.h"
#include "EntityStatesFactory.h"
#include <Temporal\Input\Input.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	const float DynamicEntity::WALK_FORCE = 2.0f;
	const float DynamicEntity::JUMP_FORCE = 15.0f;

	const int DynamicEntity::JUMP_SENSOR = 0;
	const int DynamicEntity::HANG_SENSOR = 1;
	const int DynamicEntity::BACK_EDGE_SENSOR = 2;
	const int DynamicEntity::FRONT_EDGE_SENSOR = 3;

	DynamicEntity::DynamicEntity(const EntityController* const controller, DynamicBody& body, const SpriteSheet& spritesheet, VisualLayer::Type visualLayer, EntityStateID::Type initialState)
		: _controller(controller), _body(body), _state(NULL), VisualEntity(spritesheet, visualLayer) { changeState(initialState); }

	void DynamicEntity::changeState(EntityStateID::Type stateType)
	{
		if(_state != NULL)
		{
			_state->exit(*this);
			EntityStatesFactory::get().releaseState(_state);
		}
		_state = EntityStatesFactory::get().getState(stateType);
		_state->enter(*this);
	}

	void DynamicEntity::update(void)
	{
		_state->update(*this);
		VisualEntity::update();
	}

	const Vector& DynamicEntity::getCenter(void) const { return _state->getDrawCenter(*this); }
	Orientation::Type DynamicEntity::getOrientation(void) const { return getBody().getOrientation(); }

	bool DynamicEntity::isMovingForward(void) const
	{
		return ((getBody().getOrientation() == Orientation::LEFT && getController().isLeft()) ||
				(getBody().getOrientation() == Orientation::RIGHT && getController().isRight()));
	}

	bool DynamicEntity::isMovingBackward(void) const
	{
		return ((getBody().getOrientation() == Orientation::RIGHT && getController().isLeft()) ||
				(getBody().getOrientation() == Orientation::LEFT && getController().isRight()));
	}
}
