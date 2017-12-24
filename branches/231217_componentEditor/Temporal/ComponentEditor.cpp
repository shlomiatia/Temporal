#include "ComponentEditor.h"
#include "Shapes.h"
#include "Graphics.h"
#include "Utils.h"
#include "Delegate.h"
#include "Control.h"
#include <algorithm>

namespace Temporal
{
	static const Hash PREVIOUS_BUTTON_ID("ENT_BUTTON_PREVIOUS_PAGE");
	static const Hash NEXT_BUTTON_ID("ENT_BUTTON_NEXT_PAGE");
	static const unsigned int ROW_CONTROLS = 24;
	static const unsigned int ROW_EDIT_CONTROLS = ROW_CONTROLS - 1;

	void ComponentEditor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
			const float COLUMN_CONTROLS = 2.0f;
			
			CONTROL_SIZE = Vector(WINDOW_SIZE.getX() / COLUMN_CONTROLS, WINDOW_SIZE.getY() / ROW_CONTROLS);

			float y = WINDOW_SIZE.getY() - CONTROL_SIZE.getY() / 2.0f;

			Control* previousButton = addButton(PREVIOUS_BUTTON_ID, AABB(Vector(getLeftControlX(), y), CONTROL_SIZE / 2.0f), "Previous Page", createAction(ComponentEditor, previousPage), Key::PAGE_UP);
			previousButton->setBackgroundColor(Color(0.984375f, 0.2890625f, 0.1015625f, 0.9f));


			Control* nextButton = addButton(NEXT_BUTTON_ID, AABB(Vector(getRightControlX(), y), CONTROL_SIZE / 2.0f), "Next Page", createAction(ComponentEditor, nextPage), Key::PAGE_DOWN);
			nextButton->setBackgroundColor(Color(0.984375f, 0.2890625f, 0.1015625f, 0.9f));

			_componentEditorSerializer.serialize("entity", _entity);

			setPage(0);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			for (HashIterator i = _ids.begin(); i != _ids.end(); ++i)
			{
				getEntity().getManager().remove(*i);
			}
			getEntity().getManager().remove(PREVIOUS_BUTTON_ID);
			getEntity().getManager().remove(NEXT_BUTTON_ID);
			_ids.clear();
		}
	}

	void ComponentEditor::addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent)
	{
		addLabel(AABB(Vector(getLeftControlX(), getControlY()), CONTROL_SIZE / 2.0f), label);
		Hash id(Utils::format("%s_TEXTBOX", label).c_str());
		Control* control = addTextBox(id, AABB(Vector(getRightControlX(), getControlY()), CONTROL_SIZE / 2.0f), text, textChangedEvent);
		_ids.push_back(Hash(label));
		_ids.push_back(id);
	}

	void ComponentEditor::addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent)
	{
		addLabel(AABB(Vector(getLeftControlX(), getControlY()), CONTROL_SIZE / 2.0f), label);
		Hash id(Utils::format("%s_CHECKBOX", label).c_str());
		addCheckBox(id, AABB(Vector(getRightControlX(), getControlY()), CONTROL_SIZE / 2.0f), value, checkChangedEvent);
		_ids.push_back(Hash(label));
		_ids.push_back(id);
	}

	float ComponentEditor::getLeftControlX() const
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		float x = (WINDOW_SIZE.getX() - CONTROL_SIZE.getX()) / 2.0f;
		return x;
	}

	float ComponentEditor::getRightControlX() const
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		float x = (WINDOW_SIZE.getX() + CONTROL_SIZE.getX()) / 2.0f;
		return x;
	}

	float ComponentEditor::getControlY() const
	{
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		float y = WINDOW_SIZE.getY() - CONTROL_SIZE.getY() - (_ids.size() / 2 % ROW_EDIT_CONTROLS) * CONTROL_SIZE.getY() - CONTROL_SIZE.getY() / 2.0f;
		return y;
	}

	void ComponentEditor::previousPage()
	{
		int currentPage = getPage();
		if (currentPage > 0)
		{
			--currentPage;
		}
		setPage(currentPage);
	}

	void ComponentEditor::nextPage()
	{
		int currentPage = getPage();
		int pages = _ids.size() / 2 / ROW_EDIT_CONTROLS;
		if (currentPage < pages)
		{
			currentPage++;
		}
		setPage(currentPage);
	}

	int ComponentEditor::getPage() const
	{
		for (int i = 0; i < _ids.size(); ++i)
		{
			Hash id = _ids[i];
			Control& control = *static_cast<Control*>(getEntity().getManager().getEntity(id)->get(ComponentsIds::CONTROL));
			if (control.isVisible())
			{
				return i / 2 / ROW_EDIT_CONTROLS;
			}
		}
		abort();
	}

	void ComponentEditor::setPage(int page)
	{
		for (HashIterator i = _ids.begin(); i != _ids.end(); ++i)
		{
			Control& control = *static_cast<Control*>(getEntity().getManager().getEntity(*i)->get(ComponentsIds::CONTROL));
			control.setVisible(false);
		}

		for (int i = page * ROW_EDIT_CONTROLS * 2; i < std::min((page + 1) * ROW_EDIT_CONTROLS * 2, _ids.size()); ++i)
		{
			Hash id = _ids[i];
			Control& control = *static_cast<Control*>(getEntity().getManager().getEntity(id)->get(ComponentsIds::CONTROL));
			control.setVisible(true);
		}
	}
}
