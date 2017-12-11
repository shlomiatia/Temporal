#include "DoorEditor.h"
#include "Door.h"

namespace Temporal
{
	Hash DoorEditor::TYPE("door-editor");

	void DoorEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox("Opened", _door.isOpened(), createAction1(DoorEditor, bool, openedChanged));
		}
	}

	void DoorEditor::openedChanged(bool b)
	{
		_door.setOpened(b);
	}
}