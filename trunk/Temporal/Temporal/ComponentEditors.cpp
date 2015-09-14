#include "ComponentEditors.h"
#include "MovingPlatform.h"
#include "Shapes.h"
#include "Graphics.h"
#include "Utils.h"
#include "Delegate.h"
#include "Button.h"
#include "Laser.h"
#include "Lighting.h"
#include "Door.h"
#include "Control.h"
#include "TemporalPeriod.h"

namespace Temporal
{
	static const float PADDING = 16.0f;
	static const Hash OK_BUTTON_ID = Hash("ENT_BUTTON_OK");

	/*
	 * Component editor
	 */
	void ComponentEditor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
			PANEL_SIZE = WINDOW_SIZE / 1.5f;
			CONTROL_SIZE = Vector((PANEL_SIZE.getX() - PADDING * 2.0f) / 1.0f, (PANEL_SIZE.getY() - PADDING * 4.0f) / 3.0f);
			//	addControl(Hash("ENT_PANEL"), AABB(WINOW_CENTER, PANEL_SIZE / 2.0f));
			
			_y = PANEL_SIZE.getY() + (WINDOW_SIZE.getY() - PANEL_SIZE.getY()) / 2.0f;
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			ToolComponent::addButton(OK_BUTTON_ID, getNextControlShape(), "Ok", createAction(ComponentEditor, ok));
			_ids.push_back(OK_BUTTON_ID);
		}
	}

	void ComponentEditor::addPanelTextBox(Hash id, const char* text, IAction1<const char*>* textChangedEvent)
	{
		Control* control = addTextBox(id, getNextControlShape(), text, textChangedEvent);
		if (!_focused)
		{
			control->focus();
			_focused = true;
		}
		_ids.push_back(id);
	}

	void ComponentEditor::addPanelCheckBox(Hash id, bool value, IAction1<bool>* checkChangedEvent)
	{
		addCheckBox(id, getNextControlShape(), value, checkChangedEvent);
		_ids.push_back(id);
	}

	AABB ComponentEditor::getNextControlShape()
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		const AABB CONTROL_SHAPE(Vector(WINDOW_SIZE.getX() / 2.0f, _y - PADDING - CONTROL_SIZE.getY() / 2.0f), CONTROL_SIZE / 2.0f);
		_y -= (PADDING + CONTROL_SIZE.getY());
		return CONTROL_SHAPE;
	}

	void ComponentEditor::ok()
	{
		for (HashIterator i = _ids.begin(); i != _ids.end(); ++i)
		{
			getEntity().getManager().remove(*i);
		}
		getEntity().getManager().remove(getEntity().getId());
	}

	/*
	 * Moving platform editor
	 */
	void MovingPlatformEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
 		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox(Hash("ENT_TEXTBOX_X"), Utils::toString(_movingPlatform.getMovement().getX()).c_str(), createAction1(MovingPlatformEditor, const char*, movementXChanged));
			addPanelTextBox(Hash("ENT_TEXTBOX_Y"), Utils::toString(_movingPlatform.getMovement().getY()).c_str(), createAction1(MovingPlatformEditor, const char*, movementYChanged));
		}
	}

	void MovingPlatformEditor::movementXChanged(const char* s)
	{
		_movingPlatform.getMovement().setX(Utils::parseFloat(s));
	}

	void MovingPlatformEditor::movementYChanged(const char* s)
	{
		_movingPlatform.getMovement().setY(Utils::parseFloat(s));
	}

	/*
	 * Button editor
	 */
	void ButtonEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox(Hash("ENT_TEXTBOX_TARGET"), _button.getTarget().getString(), createAction1(ButtonEditor, const char*, targetChanged));
		}
	}

	void ButtonEditor::targetChanged(const char* s)
	{
		_button.setTarget(Hash(s));
	}

	/*
	* Laser editor
	*/
	void LaserEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox(Hash("ENT_TEXTBOX_SPEED_PER_SECOND"), Utils::toString(_laser.getSpeedPerSecond()).c_str(), createAction1(LaserEditor, const char*, speedPerSecondChanged));
			addPanelCheckBox(Hash("ENT_CHECKBOX_FRIENDLY"), _laser.isFriendly(), createAction1(LaserEditor, bool, friendlyChanged));
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

	/*
	* Light editor
	*/
	void LightEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox(Hash("ENT_CHECKBOX_ACTIVATED"), _light.isActivated(), createAction1(LightEditor, bool, activatedChanged));
		}
	}

	void LightEditor::activatedChanged(bool b)
	{
		_light.setActivated(b);
	}

	/*
	* Door editor
	*/
	void DoorEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox(Hash("ENT_CHECKBOX_CLOSED"), _door.isClosed(), createAction1(DoorEditor, bool, closedChanged));
		}
	}

	void DoorEditor::closedChanged(bool b)
	{
		_door.setClosed(b);
	}

	/*
	* Temporal period editor
	*/
	void TemporalPeriodEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			
			addPanelTextBox(Hash("ENT_TEXTBOX_FUTURE_SELF_ID"), _period.getFutureSelfId().getString(), createAction1(TemporalPeriodEditor, const char*, futureSelfIdChanged));
		}
	}

	void TemporalPeriodEditor::futureSelfIdChanged(const char* s)
	{
		_period.setFutureSelfId(Hash(s));
	}
}