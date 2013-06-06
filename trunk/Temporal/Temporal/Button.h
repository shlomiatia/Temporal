#ifndef BUTTON_H
#define BUTTON_H

#include "Hash.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Button : public Component
	{
	public:
		Button(const Hash& target) : _target(target) {}

		ComponentType::Enum getType() const { return ComponentType::OTHER; }
		void handleMessage(Message& message);

		Component* clone() const { return new Button(_target); }

	private:
		Hash _target;

		friend class SerializationAccess;
	};
}

#endif