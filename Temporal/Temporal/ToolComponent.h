#ifndef TOOL_COMPONENT_H
#define TOOL_COMPONENT_H

#include "EntitySystem.h"
#include "Hash.h"
#include "InputEnums.h"

namespace Temporal
{
	class Control;
	class AABB;
	class IAction;
	template<class T>
	class IAction1;

	class ToolComponent : public Component
	{
	public:
		ToolComponent() {}

	protected:
		Control* addControl(Hash id, const AABB& shape);
		Control* addLabel(const AABB& shape, const char* text);
		Control* addButton(Hash id, const AABB& shape, const char* text, IAction* commandEvent, Key::Enum shortcutKey = Key::NONE);
		Control* addTextBox(Hash id, const AABB& shape, const char* text, IAction1<const char*>* textChangedEvent);
		Control* addCheckBox(Hash id, const AABB& shape, bool value, IAction1<bool>* checkChangedEvent);
	};
}
#endif