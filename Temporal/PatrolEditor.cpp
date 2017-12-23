#include "PatrolEditor.h"
#include "Patrol.h"

namespace Temporal
{
	Hash PatrolEditor::TYPE("patrol-editor");

	void PatrolEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox("Is Static", _patrol.isStatic(), createAction1(PatrolEditor, bool, isStaticChanged));
			addPanelTextBox("Security Camera Id", _patrol.getSecurityCameraId().getString(), createAction1(PatrolEditor, const char*, securityCameraIdChanged));
		}
	}

	void PatrolEditor::isStaticChanged(bool b)
	{
		_patrol.setStatic(b);
	}

	void PatrolEditor::securityCameraIdChanged(const char* s)
	{
		_patrol.setSecurityCameraId(Hash(s));
	}
}