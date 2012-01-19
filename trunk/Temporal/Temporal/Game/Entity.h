#pragma once

namespace Temporal { class EntityState; }

#include <Temporal\Base\Base.h>
#include <Temporal\Physics\Body.h>
#include <Temporal\Graphics\Sprite.h>
#include "EntityController.h"
#include "EntityState.h"
#include "EntityStateID.h"

namespace Temporal
{
	class Entity
	{
	public:
		static const float WALK_FORCE;
		static const float JUMP_FORCE;
		static const float CLIMBE_FORCE;

		static const int JUMP_SENSOR;
		static const int HANG_SENSOR;
		static const int BACK_EDGE_SENSOR;
		static const int FRONT_EDGE_SENSOR;

		Entity(const EntityController* const controller, Body& body, const SpriteSheet& spritesheet);
		~Entity(void) { delete _controller; }

		const EntityController& getController(void) const { return *_controller; }
		Body& getBody(void) { return _body; }
		const Body& getBody(void) const { return _body; }
		Sprite& getSprite(void) { return _sprite; }

		void changeState(EntityStateID::Type stateType);
		void update(void);
		void draw(void) { getSprite().draw(getBody().getBounds().getCenter(), getBody().getOrientation()); }

		bool isMovingForward(void) const;
		bool isMovingBackward(void) const;
	private:
		const EntityController* const _controller;
		Body& _body;
		Sprite _sprite;
		EntityState* _state;

		Entity(const Entity&);
		Entity& operator=(const Entity&);
	};
}
