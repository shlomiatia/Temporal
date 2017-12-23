#ifndef DOOR_EDITOR_H
#define DOOR_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class Door;

	class DoorEditor : public ComponentEditor
	{
	public:
		DoorEditor(Door& door) : _door(door) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Door& _door;

		void openedChanged(bool b);
	};
}

#endif