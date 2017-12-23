#include "TemporalPeriodEditor.h"
#include "TemporalPeriod.h"

namespace Temporal
{
	Hash TemporalPeriodEditor::TYPE("door-editor");

	void TemporalPeriodEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Future Self", _period.getEditorFutureSelfId().getString(), createAction1(TemporalPeriodEditor, const char*, futureSelfIdChanged));
			addPanelCheckBox("Create Future Self", _period.isCreateFutureSelf(), createAction1(TemporalPeriodEditor, bool, createFutureSelfChanged));
			addPanelCheckBox("Sync Future Self", _period.isSyncFutureSelf(), createAction1(TemporalPeriodEditor, bool, syncFutureSelfChanged));
		}
	}

	void TemporalPeriodEditor::futureSelfIdChanged(const char* s)
	{
		_period.setEditorFutureSelfId(Hash(s));
	}

	void TemporalPeriodEditor::createFutureSelfChanged(bool b)
	{
		_period.setCreateFutureSelf(b);
	}

	void TemporalPeriodEditor::syncFutureSelfChanged(bool b)
	{
		_period.setSyncFutureSelf(b);
	}
}