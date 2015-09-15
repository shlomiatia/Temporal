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
#include "Patrol.h"

namespace Temporal
{
	static const float PADDING = 16.0f;
	static const Hash OK_BUTTON_ID = Hash("ENT_BUTTON_OK");

	/*
	 * Component editor
	 */
	float ComponentEditor::_y = 0.0f;
	bool ComponentEditor::_focused = false;
	HashList ComponentEditor::_ids;

	void ComponentEditor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
			PANEL_SIZE = WINDOW_SIZE / 1.5f;
			const float COLUMN_CONTROLS = 2.0f;
			const float ROW_CONTROLS = 6.0f;
			CONTROL_SIZE = Vector((PANEL_SIZE.getX() - PADDING * (COLUMN_CONTROLS + 1.0f)) / COLUMN_CONTROLS, (PANEL_SIZE.getY() - PADDING * (ROW_CONTROLS + 1.0f)) / ROW_CONTROLS);
			//	addControl(Hash("ENT_PANEL"), AABB(WINOW_CENTER, PANEL_SIZE / 2.0f));
			
			ComponentList& components = getEntity().getAll();
			if (components.at(0) == this)
			{
				_y = PANEL_SIZE.getY() + (WINDOW_SIZE.getY() - PANEL_SIZE.getY()) / 2.0f;
				_focused = false;
			}
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			ComponentList& components = getEntity().getAll();
			if (components.at(components.size() - 1) == this)
			{
				ToolComponent::addButton(OK_BUTTON_ID, getNextControlShape(), "Ok", createAction(ComponentEditor, ok));
				_ids.push_back(OK_BUTTON_ID);
			}
		}
	}

	void ComponentEditor::addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent)
	{
		addLabel(getNextLabelShape(), label);
		Hash id(Utils::format("%s_TEXTBOX", label).c_str());
		Control* control = addTextBox(id, getNextControlShape(), text, textChangedEvent);
		if (!_focused)
		{
			control->focus();
			_focused = true;
		}
		_ids.push_back(Hash(label));
		_ids.push_back(id);
	}

	void ComponentEditor::addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent)
	{
		addLabel(getNextLabelShape(), label);
		Hash id(Utils::format("%s_CHECKBOX", label).c_str());
		addCheckBox(id, getNextControlShape(), value, checkChangedEvent);
		_ids.push_back(Hash(label));
		_ids.push_back(id);
	}

	AABB ComponentEditor::getNextLabelShape()
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		const AABB CONTROL_SHAPE(Vector((WINDOW_SIZE.getX() - PADDING - CONTROL_SIZE.getX()) / 2.0f, _y - PADDING - CONTROL_SIZE.getY() / 2.0f), CONTROL_SIZE / 2.0f);
		return CONTROL_SHAPE;
	}

	AABB ComponentEditor::getNextControlShape()
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		const AABB CONTROL_SHAPE(Vector((WINDOW_SIZE.getX() + PADDING + CONTROL_SIZE.getX()) / 2.0f, _y - PADDING - CONTROL_SIZE.getY() / 2.0f), CONTROL_SIZE / 2.0f);
		_y -= (PADDING + CONTROL_SIZE.getY());
		return CONTROL_SHAPE;
	}

	void ComponentEditor::ok()
	{
		for (HashIterator i = _ids.begin(); i != _ids.end(); ++i)
		{
			getEntity().getManager().remove(*i);
		}
		_ids.clear();
		getEntity().getManager().remove(getEntity().getId());
	}

	/*
	 * Moving platform editor
	 */
	Hash MovingPlatformEditor::TYPE("moving-platform-editor");

	void MovingPlatformEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
 		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Movement X", Utils::toString(_movingPlatform.getMovement().getX()).c_str(), createAction1(MovingPlatformEditor, const char*, movementXChanged));
			addPanelTextBox("Movement Y", Utils::toString(_movingPlatform.getMovement().getY()).c_str(), createAction1(MovingPlatformEditor, const char*, movementYChanged));
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

	/*
	* Laser editor
	*/
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

	/*
	* Light editor
	*/
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

	/*
	* Door editor
	*/
	Hash DoorEditor::TYPE("door-editor");

	void DoorEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox("Closed", _door.isClosed(), createAction1(DoorEditor, bool, closedChanged));
		}
	}

	void DoorEditor::closedChanged(bool b)
	{
		_door.setClosed(b);
	}

	/*
	* Temporal period editor
	*/
	Hash TemporalPeriodEditor::TYPE("door-editor");

	void TemporalPeriodEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelTextBox("Future Self", _period.getFutureSelfId().getString(), createAction1(TemporalPeriodEditor, const char*, futureSelfIdChanged));
			addPanelCheckBox("Create Future Self", _period.isCreateFutureSelf(), createAction1(TemporalPeriodEditor, bool, createFutureSelfChanged));
		}
	}

	void TemporalPeriodEditor::futureSelfIdChanged(const char* s)
	{
		_period.setFutureSelfId(Hash(s));
	}

	void TemporalPeriodEditor::createFutureSelfChanged(bool b)
	{
		_period.setCreateFutureSelf(b);
	}

	/*
	* Patrol editor
	*/
	Hash PatrolEditor::TYPE("patrol-editor");

	void PatrolEditor::handleMessage(Message& message)
	{
		ComponentEditor::handleMessage(message);
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			addPanelCheckBox("Is Static", _patrol.isStatic(), createAction1(PatrolEditor, bool, isStaticChanged));
		}
	}

	void PatrolEditor::isStaticChanged(bool b)
	{
		_patrol.setStatic(b);
	}
}