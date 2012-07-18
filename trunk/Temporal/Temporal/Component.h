#ifndef COMPONENT_H
#define COMPONENT_H

#include "GameEnums.h"
#include <stdio.h>

namespace Temporal
{
	class Entity;
	class Message;

	class Component
	{
	public:
		Component() : _entity(NULL) {}
		virtual ~Component() {}

		void setEntity(const Entity* entity) { _entity = entity; }
		virtual ComponentType::Enum getType() const = 0;

		virtual void handleMessage(Message& message) = 0;
		virtual Component* clone() const { return NULL; }
		void* sendMessageToOwner(Message& message) const;

	private:
		const Entity* _entity;

		Component(const Component&);
		Component& operator=(const Component&);
	};
}
#endif