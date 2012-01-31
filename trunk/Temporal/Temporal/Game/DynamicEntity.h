#pragma once

namespace Temporal { class EntityState; }

#include <Temporal\Base\Base.h>
#include <Temporal\Physics\DynamicBody.h>
#include <Temporal\Graphics\Animator.h>
#include "EntityController.h"
#include "EntityState.h"
#include "Enums.h"
#include "VisualEntity.h"

namespace Temporal
{
	class DynamicEntity : public VisualEntity
	{
	public:
		static const float WALK_FORCE;
		static const float JUMP_FORCE;

		static const int JUMP_SENSOR;
		static const int HANG_SENSOR;
		static const int BACK_EDGE_SENSOR;
		static const int FRONT_EDGE_SENSOR;

		DynamicEntity(const EntityController* const controller, DynamicBody& body, const SpriteSheet& spritesheet, VisualLayer::Type visualLayer, EntityStateID::Type initialState);
		~DynamicEntity(void) { delete _controller; }

		const EntityController& getController(void) const { return *_controller; }
		const DynamicBody& getBody(void) const { return _body; }
		DynamicBody& getBody(void) { return _body; }

		void changeState(EntityStateID::Type stateType);

		virtual const Vector& getCenter(void) const;
		virtual Orientation::Type getOrientation(void) const;

		virtual void update(void);

		bool isMovingForward(void) const;
		bool isMovingBackward(void) const;

	private:
		const EntityController* const _controller;
		DynamicBody& _body;
		EntityState* _state;
	};
}
