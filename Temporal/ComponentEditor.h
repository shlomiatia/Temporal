#ifndef COMPONENT_EDITOR_H
#define COMPONENT_EDITOR_H

#include "EntitySystem.h"
#include "ToolComponent.h"
#include "Vector.h"
#include "ComponentEditorSerializer.h"

namespace Temporal
{
	class ComponentEditor : public ToolComponent
	{
	public:
		ComponentEditor(Entity& entity) : ToolComponent(), _y(0.0f), _focused(false), _entity(entity), _componentEditorSerializer(*this) {}

		Hash getType() const { return Hash("component-editor"); }
		void handleMessage(Message& message);
		Component* clone() const { return 0; } 

		void addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent);
		void addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent);
	private:
		Vector PANEL_SIZE;
		Vector CONTROL_SIZE;

		HashList _ids;
		float _y;
		bool _focused;
		Entity& _entity;
		ComponentEditorSerializer _componentEditorSerializer;

		AABB getNextLabelShape();
		AABB getNextControlShape();
		void ok();
	};
}

#endif