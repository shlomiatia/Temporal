#pragma once

namespace Temporal { class Entity; }

#include "Enums.h"
#include "Message.h"
#include "Entity.h"

namespace Temporal
{
	class Component
	{
	public:
		Component(void) {}
		virtual ~Component(void) {}
		void setEntity(Entity* entity) { _entity = entity; }

		virtual ComponentType::Type getType(void) const = 0;
		virtual void handleMessage(Message& message) = 0;

	protected:
		void sendMessage(Message& message) const;

	private:
		Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};
}