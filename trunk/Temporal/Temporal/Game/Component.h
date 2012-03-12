#pragma once

#include "Enums.h"
#include <stdio.h>

namespace Temporal
{
	class Entity;
	class Message;

	class Component
	{
	public:
		Component(void) : _entity(NULL) {}
		virtual ~Component(void) {}

		void setEntity(const Entity* entity) { _entity = entity; }
		virtual ComponentType::Enum getType(void) const = 0;

		virtual void handleMessage(Message& message) = 0;
		const void* const sendMessageToOwner(Message& message) const;

	private:
		const Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};
}