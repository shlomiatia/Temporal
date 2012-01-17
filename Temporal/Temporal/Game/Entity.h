#pragma once

namespace Temporal { class EntityState; }

#include <Temporal\Base\Base.h>
#include <Temporal\Physics\Body.h>
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

		Entity(Body& body, const EntityController* controller);
		~Entity(void) { delete _controller; }

		Body& getBody(void) { return _body; }
		const Body& getBody(void) const { return _body; }
		const EntityController& getController(void) const { return *_controller; }

		void changeState(EntityStateID::Type stateType);
		void update(void);

		bool isMovingForward(void) const;
		bool isMovingBackward(void) const;
	private:
		Body& _body;
		const EntityController* const _controller;
		EntityState* _state;

		Entity(const Entity&);
		Entity& operator=(const Entity&);
	};
}
