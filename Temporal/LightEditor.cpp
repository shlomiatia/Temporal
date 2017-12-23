#include "LightEditor.h"
#include "Lighting.h"

namespace Temporal
{
	Hash LightEditor::TYPE("light-editor");

	void LightEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox("Activated", _light.isActivated(), createAction1(LightEditor, bool, activatedChanged));
		}
	}

	void LightEditor::activatedChanged(bool b)
	{
		_light.setActivated(b);
	}
}