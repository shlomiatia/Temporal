#include "ToolComponent.h"
#include "Shapes.h"
#include "Transform.h"
#include "Control.h"
#include "GameState.h"

namespace Temporal
{
	Control* ToolComponent::addControl(Hash id, const AABB& shape)
	{
		Transform* transform = new Transform(shape.getCenter());
		Control* control = new Control();
		control->setWidth(shape.getWidth());
		control->setHeight(shape.getHeight());
		Entity* entity = new Entity(id);
		entity->add(transform);
		entity->add(control);
		
		getEntity().getManager().add(entity);
		return control;
	}

	Control* ToolComponent::addLabel(const AABB& shape, const char* text)
	{
		Control* control = addControl(Hash(text), shape);
		control->setText(text);
		return control;
	}

	Control* ToolComponent::addButton(Hash id, const AABB& shape, const char* text, IAction* commandEvent, Key::Enum shortcutKey)
	{
		Control* control = addControl(id, shape);
		control->setText(text);
		control->setCommandEvent(commandEvent);
		control->setShortcutKey(shortcutKey);
		return control;
	}

	Control* ToolComponent::addTextBox(Hash id, const AABB& shape, const char* text, IAction1<const char*>* textChangedEvent)
	{
		Control* control = addControl(id, shape);
		control->setText(text);
		control->setTextChangedEvent(textChangedEvent);
		return control;
	}
}