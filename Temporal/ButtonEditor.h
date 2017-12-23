#ifndef BUTTON_EDITOR_H
#define BUTTON_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class Button;
	
	class ButtonEditor : public ComponentEditor
	{
	public:
		ButtonEditor(Button& button) : _button(button) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Button& _button;

		void targetChanged(const char* s);
	};
}

#endif