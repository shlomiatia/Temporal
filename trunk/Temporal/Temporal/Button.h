#ifndef BUTTON_H
#define BUTTON_H

#include "Hash.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Button : public Component
	{
	public:
		Button(const Hash& target = Hash::INVALID) : _target(target) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Button(_target); }

		static const Hash TYPE;
	private:

		Hash _target;

		friend class SerializationAccess;
	};
}

#endif