#include "ComponentEditor.h"
#include "Shapes.h"
#include "Graphics.h"
#include "Utils.h"
#include "Delegate.h"
#include "Control.h"

namespace Temporal
{
	static const float PADDING = 0;
	static const Hash OK_BUTTON_ID = Hash("ENT_BUTTON_OK");

	void ComponentEditor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
			PANEL_SIZE = WINDOW_SIZE;
			const float COLUMN_CONTROLS = 2.0f;
			const float ROW_CONTROLS = 24.0f;
			CONTROL_SIZE = Vector((PANEL_SIZE.getX() - PADDING * (COLUMN_CONTROLS + 1.0f)) / COLUMN_CONTROLS, (PANEL_SIZE.getY() - PADDING * (ROW_CONTROLS + 1.0f)) / ROW_CONTROLS);
			
			ComponentList& components = getEntity().getAll();
			if (components.at(0) == this)
			{
				_y = PANEL_SIZE.getY() + (WINDOW_SIZE.getY() - PANEL_SIZE.getY()) / 2.0f;
				_focused = false;
			}
			_componentEditorSerializer.serialize("entity", _entity);
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			ComponentList& components = getEntity().getAll();
			if (components.at(components.size() - 1) == this)
			{
				Control* okButton = addButton(OK_BUTTON_ID, getNextControlShape(), "Ok", createAction(ComponentEditor, ok));
				okButton->setBackgroundColor(Color(0.984375f, 0.2890625f, 0.1015625f, 0.9f));
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
}