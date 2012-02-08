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
		Component(void) : _entity(NULL) {}
		virtual ~Component(void) {}

		void setEntity(Entity* entity) { _entity = entity; }
		virtual ComponentType::Enum getType(void) const = 0;

		virtual void handleMessage(Message& message) = 0;
		void sendMessageToOwner(Message& message) const;

		// TODO: Remove all templates SLOTH!
		template <class T>
		const T& sendQueryMessageToOwner(Message& query) const
		{
			sendMessageToOwner(query);
			return query.getParam<T>();
		}

	private:
		Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};
}