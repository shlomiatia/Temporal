#ifndef BUTTON_H
#define BUTTON_H

#include "Hash.h"
#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Button : public Component
	{
	public:
		Button(const Hash& target = Hash::INVALID) : _target(target) {}

		Hash getType() const { return ComponentsIds::BUTTON; }
		void handleMessage(Message& message);

		Component* clone() const { return new Button(_target); }
		void setTarget(Hash target) { _target = target; }
		Hash getTarget() const { return _target; }

		
	private:
		Hash _target;

		friend class SerializationAccess;
	};
}

#endif