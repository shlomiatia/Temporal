#include "ButtonEditor.h"
#include "Button.h"

namespace Temporal
{
	Hash ButtonEditor::TYPE("button-editor");

	void ButtonEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Target", _button.getTarget().getString(), createAction1(ButtonEditor, const char*, targetChanged));
		}
	}

	void ButtonEditor::targetChanged(const char* s)
	{
		_button.setTarget(Hash(s));
	}
}