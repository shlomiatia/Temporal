#include "ActivationSensorEditor.h"
#include "ActivationSensor.h"

namespace Temporal
{
	Hash ActivationSensorEditor::TYPE("activation-sensor-editor");

	void ActivationSensorEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Target", _activationSensor.getTarget().getString(), createAction1(ActivationSensorEditor, const char*, targetChanged));
		}
	}

	void ActivationSensorEditor::targetChanged(const char* s)
	{
		_activationSensor.setTarget(Hash(s));
	}
}
