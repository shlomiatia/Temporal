#include "LaserEditor.h"
#include "Laser.h"
#include "Utils.h"

namespace Temporal
{
	Hash LaserEditor::TYPE("laser-editor");

	void LaserEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Speed Per Second", Utils::toString(_laser.getSpeedPerSecond()).c_str(), createAction1(LaserEditor, const char*, speedPerSecondChanged));
			addPanelCheckBox("Friendly", _laser.isFriendly(), createAction1(LaserEditor, bool, friendlyChanged));
		}
	}

	void LaserEditor::friendlyChanged(bool b)
	{
		_laser.setFriendly(b);
	}

	void LaserEditor::speedPerSecondChanged(const char* s)
	{
		_laser.setSpeedPerSecond(Utils::parseFloat(s));
	}
}
