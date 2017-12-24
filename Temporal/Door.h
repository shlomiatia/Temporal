#ifndef DOOR_H
#define DOOR_H

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Door : public Component
	{
	public:
		Door(bool opened = false) { _opened = opened; }

		Hash getType() const { return ComponentsIds::DOOR; }
		Component* clone() const { return new Door(_opened); }
		void handleMessage(Message& message);

	private:
		bool _opened;
		void set();

		friend class SerializationAccess;
	};
}

#endif