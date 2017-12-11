#ifndef COMPONENT_EDITOR_H
#define COMPONENT_EDITOR_H

#include "EntitySystem.h"
#include "ToolComponent.h"
#include "Vector.h"

namespace Temporal
{
	

	class ComponentEditor : public ToolComponent
	{
	public:
		ComponentEditor() : ToolComponent(20) {}

		void handleMessage(Message& message);

	protected:
		void addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent);
		void addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent);
	private:
		Vector PANEL_SIZE;
		Vector CONTROL_SIZE;

		static HashList _ids;
		static float _y;
		static bool _focused;

		AABB getNextLabelShape();
		AABB getNextControlShape();
		void ok();
	};
}

#endif